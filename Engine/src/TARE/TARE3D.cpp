#include "tagepch.h"
#include "TARE3D.h"

#include "TAGE/Application/Application.h"
#include "API/RenderCommand.h"

namespace TARE {
	ShaderLibrary TARE3D::_ShaderLibrary;
	SceneData TARE3D::_SceneData;
	UniformBufferDirectionalLight TARE3D::_UBODirectionalLight;
	UniformBufferPointLights TARE3D::_UBOPointLights;
	UniformBufferSpotLights TARE3D::_UBOSpotLights;
	TAGE::MEM::Ref<Framebuffer> TARE3D::_GBuffer;
	TAGE::MEM::Ref<Framebuffer> TARE3D::_FinalBuffer;
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

	void TARE3D::Init(){
		uint width, height;
		width = TAGE::Application::Get()->GetWindow()->GetWidth();
		height = TAGE::Application::Get()->GetWindow()->GetHeight();

		RenderCommand::Create();
		RenderCommand::DepthTest(true);
		Grid::Create();

		GetShaderLibrary().LoadShader("DeferredGBuffer", "Resources/Shaders/DeferredGBuffer.glsl");
		GetShaderLibrary().LoadShader("DeferredLightingPBR", "Resources/Shaders/DeferredLightingPBR.glsl");
		GetShaderLibrary().LoadShader("ForwardBase", "Resources/Shaders/ForwardBase.glsl");
		GetShaderLibrary().LoadShader("Grid", "Resources/Shaders/Grid.glsl");
		GetShaderLibrary().LoadShader("EquirectangularCubemap", "Resources/Shaders/EquirectangularCubemap.glsl");
		GetShaderLibrary().LoadShader("IrradianceConvolution", "Resources/Shaders/IrradianceConvolution.glsl");
		GetShaderLibrary().LoadShader("Background", "Resources/Shaders/Background.glsl");

		GetShaderLibrary().GetShader("DeferredLightingPBR")->Use();
		GetShaderLibrary().GetShader("DeferredLightingPBR")->CreateUBO("CameraData", sizeof(CameraUniformBufferData), 5);
		GetShaderLibrary().GetShader("DeferredLightingPBR")->CreateUBO("DirectionalLightData", sizeof(UniformBufferDirectionalLight), 6);
		GetShaderLibrary().GetShader("DeferredLightingPBR")->CreateUBO("PointLightData",       sizeof(UniformBufferPointLights), 7);
		GetShaderLibrary().GetShader("DeferredLightingPBR")->CreateUBO("SpotLightData",        sizeof(UniformBufferSpotLights), 8);

		_GBuffer = Framebuffer::Create({
			{ 
				TextureFormat::RGBA16F, // Position
				TextureFormat::RGBA16F, // Normal
				TextureFormat::RGBA8,   // Albedo
				TextureFormat::RGBA8,   // Metallic Roughness
				TextureFormat::RED_INTEGER, // Entity ID
				TextureFormat::DEPTH24
			},
			1,
			width, height
			});
		_FinalBuffer = Framebuffer::Create({ { TextureFormat::RGBA16F }, 1, width, height });
	}

	void TARE3D::Destroy() {
		RenderCommand::Destroy();
	}

	void TARE3D::Resize(uint width, uint height) {
		_GBuffer->Resize(width, height);
		_FinalBuffer->Resize(width, height);
	}

	void TARE3D::BeginDeferredRender(const TAGE::MEM::Ref<Camera>& camera)
	{
		_SceneData.CameraData.ViewMatrix = camera->GetViewMatrix();
		_SceneData.CameraData.ProjectionMatrix = camera->GetProjectionMatrix();
		_SceneData.CameraData.InversedProjectionMatrix = glm::inverse(camera->GetProjectionMatrix());
		_SceneData.CameraData.InversedViewMatrix = glm::inverse(camera->GetViewMatrix());
		_SceneData.CameraData.ViewProjectionMatrix = camera->GetProjectionMatrix() * camera->GetViewMatrix();
		_SceneData.CameraData.CameraPosition = camera->GetPosition();
		_SceneData.CameraData.CameraDirection = camera->GetEulerRotation();
		_SceneData.CameraData.CameraUp = camera->GetUp();

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

		UniformBufferPointLights& UBOPointLights = _UBOPointLights;
		UniformBufferSpotLights& UBOSpotLights = _UBOSpotLights;
		UBOPointLights.Count = (uint)_SceneData.LightEnviroment.PointLights.size();
		std::memcpy(UBOPointLights.PointLights, _SceneData.LightEnviroment.PointLights.data(), sizeof(PointLight) * UBOPointLights.Count);
		UBOSpotLights.Count = (uint)_SceneData.LightEnviroment.SpotLights.size();
		std::memcpy(UBOSpotLights.SpotLights, _SceneData.LightEnviroment.SpotLights.data(), sizeof(SpotLight) * UBOSpotLights.Count); // Move this to another function or class to handle all scene rendering

		auto& lightShader = GetShaderLibrary().GetShader("DeferredLightingPBR");
		const char* attachments[4] = { 
			"u_gPos",
			"u_gNormal",
			"u_gAlbedo",
			"u_gMetallicRoughness"
		};
		_FinalBuffer->Bind();
		lightShader->Use();
		lightShader->UpdateUBO(5, &_SceneData.CameraData, sizeof(CameraUniformBufferData));
		lightShader->UpdateUBO(6, &_SceneData.LightEnviroment.DirectionalLight,  sizeof(UniformBufferDirectionalLight));
		lightShader->UpdateUBO(7, &UBOPointLights,  16ull + sizeof(PointLight) * UBOPointLights.Count);
		lightShader->UpdateUBO(8, &UBOSpotLights,   16ull + sizeof(SpotLight)  *  UBOSpotLights.Count);

		for (int i = 0; i < 4; i++) {
			_GBuffer->BindColorAttachment(i, i);
			lightShader->SetUniform(attachments[i], i);
		}
		if (_SceneData.Enviroment) {
			_SceneData.Enviroment->RadianceMap->Bind(5);
			lightShader->SetUniform("u_RadianceMap", 5);
			_SceneData.Enviroment->IrradianceMap->Bind(6);
			lightShader->SetUniform("u_IrradianceMap", 6);
		}

		//TARE3D::DrawSkybox();
		RenderCommand::BlendTest(true);
		RenderCommand::Clear(0x00004000 | 0x00000100); // Change this
		RenderCommand::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
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
	void TARE3D::DrawSkybox()
	{
		if (_SceneData.Enviroment) {
			auto& shader = GetShaderLibrary().GetShader("Background");
			shader->Use();
			_SceneData.Enviroment->RadianceMap->Bind(0);
			shader->SetUniform("u_Projection", _SceneData.CameraData.ProjectionMatrix);
			shader->SetUniform("u_View", _SceneData.CameraData.ViewMatrix);
			shader->SetUniform("u_EnviromentMap", 0);
			RenderCommand::RenderCube();
		}
	}
	TAGE::MEM::Ref<TARE::TextureCube> TARE3D::EquirectangularToCubemap(TAGE::MEM::Ref<TARE::Texture2D> equirectangularMap)
	{
		TARE::TextureSpecs cubeSpecs;
		cubeSpecs.Width = 512;
		cubeSpecs.Height = 512;
		cubeSpecs.CreateMipmaps = false;
		cubeSpecs.format = TARE::TextureFormat::RGB;
		cubeSpecs.DataType = TARE::TextureDataType::Float;
		cubeSpecs.InternalFormat = TARE::TextureFormat::RGB16F;

		TAGE::MEM::Ref<TARE::TextureCube> cubemap = TARE::TextureCube::Create(cubeSpecs);
		TAGE::MEM::Ref<TARE::Framebuffer> captureFBO = TARE::Framebuffer::Create({ { TARE::TextureFormat::RGB16F, TARE::TextureFormat::DEPTH24 }, 1, 512, 512 });

		auto shader = TARE3D::GetShaderLibrary().GetShader("EquirectangularCubemap");
		shader->Use();
		shader->SetUniform("u_EquirectangularMap", 0);
		shader->SetUniform("u_Projection", captureProjection);

		equirectangularMap->Bind(0);
		captureFBO->Bind();
		for (uint i = 0; i < 6; ++i) {
			shader->SetUniform("u_View", captureViews[i]);
			captureFBO->AttachCubemapFace(cubemap, i);
			RenderCommand::Clear(0x00004000 | 0x00000100);
			RenderCommand::RenderCube();
		}
		captureFBO->Unbind();
		cubemap->GenerateMipmap();

		return cubemap;
	}
	TAGE::MEM::Ref<TARE::TextureCube> TARE3D::CreateIrradianceMap(TAGE::MEM::Ref<TARE::TextureCube> envCubemap)
	{
		TARE::TextureSpecs irradianceSpecs;
		irradianceSpecs.Width = 32;
		irradianceSpecs.Height = 32;
		irradianceSpecs.format = TARE::TextureFormat::RGB;
		irradianceSpecs.DataType = TARE::TextureDataType::Float;
		irradianceSpecs.InternalFormat = TARE::TextureFormat::RGB16F;

		TAGE::MEM::Ref<TARE::TextureCube> irradianceMap = TARE::TextureCube::Create(irradianceSpecs);
		TAGE::MEM::Ref<TARE::Framebuffer> captureFBO = TARE::Framebuffer::Create({ { TARE::TextureFormat::RGB16F, TARE::TextureFormat::DEPTH24 }, 1, 32, 32 });

		auto shader = TARE3D::GetShaderLibrary().GetShader("IrradianceConvolution");
		shader->Use();
		shader->SetUniform("u_EnvironmentMap", 0);
		shader->SetUniform("u_Projection", captureProjection);

		envCubemap->Bind(0);
		captureFBO->Bind();
		for (uint i = 0; i < 6; ++i)
		{
			shader->SetUniform("u_View", captureViews[i]);
			captureFBO->AttachCubemapFace(irradianceMap, i);
			RenderCommand::Clear(0x00004000 | 0x00000100);
			RenderCommand::RenderCube();
		}
		captureFBO->Unbind();

		return irradianceMap;
	}
}