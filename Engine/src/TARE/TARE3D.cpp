#include "tagepch.h"
#include "TARE3D.h"

#include "TAGE/Application/Application.h"
#include "API/RenderCommand.h"

namespace TARE {
	ShaderLibrary TARE3D::_ShaderLibrary;
	SceneData TARE3D::_SceneData;
	TAGE::MEM::Ref<Framebuffer> TARE3D::_GBuffer;
	TAGE::MEM::Ref<Framebuffer> TARE3D::_FinalBuffer;

	void TARE3D::Init(){
		uint width, height;
		width = TAGE::Application::Get()->GetWindow()->GetWidth();
		height = TAGE::Application::Get()->GetWindow()->GetWidth();

		RenderCommand::Create();
		RenderCommand::DepthTest(true);
		Grid::Create();

		GetShaderLibrary().LoadShader("DeferredGBuffer", "Resources/Shaders/DeferredGBuffer.glsl");
		GetShaderLibrary().LoadShader("DeferredLightingPBR", "Resources/Shaders/DeferredLightingPBR.glsl");
		GetShaderLibrary().LoadShader("ForwardBase", "Resources/Shaders/ForwardBase.glsl");
		GetShaderLibrary().LoadShader("Grid", "Resources/Shaders/Grid.glsl");

		GetShaderLibrary().GetShader("ForwardBase")->Use();
		GetShaderLibrary().GetShader("ForwardBase")->CreateUBO("CameraData", sizeof(CameraUniformBufferData), 5);

		_GBuffer = Framebuffer::Create({
			{ 
				FramebufferTextureFormat::RGBA16F, // Position
				FramebufferTextureFormat::RGBA16F, // Normal
				FramebufferTextureFormat::RGBA8,   // Albedo
				FramebufferTextureFormat::RGBA8,   // Metallic Roughness
				FramebufferTextureFormat::RED_INTEGER, // Entity ID
				FramebufferTextureFormat::DEPTH24
			},
			1,
			width, height
			});
		_FinalBuffer = Framebuffer::Create({ { FramebufferTextureFormat::RGBA16F }, 1, width, height });
	}

	void TARE3D::Destroy() {
		RenderCommand::Destroy();
	}

	void TARE3D::Resize(uint width, uint height) {
		_GBuffer->Resize(width, height);
		_FinalBuffer->Resize(width, height);
	}

	void TARE3D::BeginForwardRender(const TAGE::MEM::Ref<Camera>& camera)
	{
		_SceneData.CameraData.ViewMatrix = camera->GetViewMatrix();
		_SceneData.CameraData.ProjectionMatrix = camera->GetProjectionMatrix();
		_SceneData.CameraData.InversedProjectionMatrix = glm::inverse(camera->GetProjectionMatrix());
		_SceneData.CameraData.InversedViewMatrix = glm::inverse(camera->GetViewMatrix());
		_SceneData.CameraData.ViewProjectionMatrix = camera->GetProjectionMatrix() * camera->GetViewMatrix();
		_SceneData.CameraData.CameraPosition = camera->GetPosition();
		_SceneData.CameraData.CameraDirection = camera->GetEulerRotation();
		_SceneData.CameraData.CameraUp = camera->GetUp();

		RenderCommand::Clear(0x00004000 | 0x00000100); // Change this
		RenderCommand::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		auto& shader = GetShaderLibrary().GetShader("ForwardBase");
		shader->Use();
		shader->UpdateUBO(5, &_SceneData.CameraData, sizeof(CameraUniformBufferData));
		Grid::Draw();
	}

	void TARE3D::EndForwardRender()
	{

	}

	void TARE3D::BeginDeferredRender(const TAGE::MEM::Ref<Camera>& camera)
	{
		auto& gBufferShader = GetShaderLibrary().GetShader("DeferredGBuffer");
		_GBuffer->Bind();
		RenderCommand::BlendTest(false);
		RenderCommand::Clear(0x00004000 | 0x00000100);
		_GBuffer->ClearAttachment(4, -1);
		gBufferShader->Use();
	}

	void TARE3D::EndDeferredRender()
	{
		_GBuffer->Unbind();

		auto& lightShader = GetShaderLibrary().GetShader("DeferredLightingPBR");
		const char* attachments[4] = { 
			"u_gPos",
			"u_gNormal",
			"u_gAlbedo",
			"u_gMetallicRoughness"
		};
		_FinalBuffer->Bind();
		lightShader->Use();
		RenderCommand::BlendTest(true);
		RenderCommand::Clear(0x00004000 | 0x00000100); // Change this
		RenderCommand::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		for (int i = 0; i < 4; i++) {
			_GBuffer->BindColorAttachment(i, i);
			lightShader->SetUniform(attachments[i], i);
		}
		RenderCommand::DrawFullScreenQuad();
		_FinalBuffer->Unbind();
	}

	void TARE3D::DrawStaticMesh(TAGE::MEM::Ref<Model> model, const glm::mat4& transform)
	{
		for (const auto& mesh : model->GetMeshes()) {
			CHECKF(mesh->GetMaterial(), "Mesh doesnt have any material");
			mesh->GetMaterial()->Use();
			GetShaderLibrary().GetShader("DeferredGBuffer")->SetUniform("u_Model", transform);
			RenderCommand::DrawIndexed(mesh->GetVertexArrayObject());
		}
	}

	void TARE3D::DrawEntityStaticMesh(TAGE::MEM::Ref<Model> model, const glm::mat4& transform, int EntityID)
	{
		for (const auto& mesh : model->GetMeshes()) {
			CHECKF(mesh->GetMaterial(), "Mesh doesnt have any material");
			mesh->GetMaterial()->Use();
			GetShaderLibrary().GetShader("DeferredGBuffer")->SetUniform("u_Model", transform);
			GetShaderLibrary().GetShader("DeferredGBuffer")->SetUniform("u_EntityID", EntityID);
			RenderCommand::DrawIndexed(mesh->GetVertexArrayObject());
		}
	}
}