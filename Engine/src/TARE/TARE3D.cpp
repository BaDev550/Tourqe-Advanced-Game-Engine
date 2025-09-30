#include "tagepch.h"
#include "TARE3D.h"

#include "API/RenderCommand.h"

namespace TARE {
	ShaderLibrary TARE3D::_ShaderLibrary;
	void TARE3D::Init(){
		RenderCommand::Create();

		GetShaderLibrary().LoadShader("TAGE_DeferredBase", "Resources/Shaders/TAGE_DeferredBase.glsl");
	}

	void TARE3D::Destroy() {
		RenderCommand::Destroy();
	}

	void TARE3D::BeginForwardRender(const TAGE::MEM::Ref<Camera>& camera)
	{
		RenderCommand::Clear(0x00004000 | 0x00000100); // Change this
		RenderCommand::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		auto& shader = GetShaderLibrary().GetShader("TAGE_DeferredBase");
		shader->Use();
		shader->SetUniform("u_View", camera->GetViewMatrix());
		shader->SetUniform("u_Projection", camera->GetProjectionMatrix());
	}

	void TARE3D::EndForwardRender()
	{

	}

	void TARE3D::DrawStaticMesh(TAGE::MEM::Ref<Model> model, const glm::mat4& transform)
	{
		for (const auto& mesh : model->GetMeshes()) {
			CHECKF(mesh->GetMaterial(), "Mesh doesnt have any material");
			mesh->GetMaterial()->Use();
			GetShaderLibrary().GetShader("TAGE_DeferredBase")->SetUniform("u_Model", transform);
			RenderCommand::DrawIndexed(mesh->GetVertexArrayObject());
		}
	}
}