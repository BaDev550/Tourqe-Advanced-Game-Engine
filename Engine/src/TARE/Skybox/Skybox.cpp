#include "tagepch.h"
#include "Skybox.h"
#include "TARE/Common/RenderCommands.h"

namespace TARE {
	Skybox::Skybox(const char* path)
	{
		_SkyboxShader = ShaderLibrary::Add("SkyboxShader", "../Engine/shaders/skybox_vertex", "../Engine/shaders/skybox_fragment");
		_SkyboxTexture = CubemapTexture::Create();
		_SkyboxTexture->LoadCubemap(path);

		_SkyboxVAO = VertexArrayObject::Create();
        std::vector<glm::vec3> skyboxVertices = {
          { -1.0f,  1.0f, -1.0f },
          { -1.0f, -1.0f, -1.0f },
          {  1.0f, -1.0f, -1.0f },
          {  1.0f, -1.0f, -1.0f },
          {  1.0f,  1.0f, -1.0f },
          { -1.0f,  1.0f, -1.0f },

          { -1.0f, -1.0f,  1.0f },
          { -1.0f, -1.0f, -1.0f },
          { -1.0f,  1.0f, -1.0f },
          { -1.0f,  1.0f, -1.0f },
          { -1.0f,  1.0f,  1.0f },
          { -1.0f, -1.0f,  1.0f },

          {  1.0f, -1.0f, -1.0f },
          {  1.0f, -1.0f,  1.0f },
          {  1.0f,  1.0f,  1.0f },
          {  1.0f,  1.0f,  1.0f },
          {  1.0f,  1.0f, -1.0f },
          {  1.0f, -1.0f, -1.0f },

          { -1.0f, -1.0f,  1.0f },
          { -1.0f,  1.0f,  1.0f },
          {  1.0f,  1.0f,  1.0f },
          {  1.0f,  1.0f,  1.0f },
          {  1.0f, -1.0f,  1.0f },
          { -1.0f, -1.0f,  1.0f },

          { -1.0f,  1.0f, -1.0f },
          {  1.0f,  1.0f, -1.0f },
          {  1.0f,  1.0f,  1.0f },
          {  1.0f,  1.0f,  1.0f },
          { -1.0f,  1.0f,  1.0f },
          { -1.0f,  1.0f, -1.0f },

          { -1.0f, -1.0f, -1.0f },
          { -1.0f, -1.0f,  1.0f },
          {  1.0f, -1.0f, -1.0f },
          {  1.0f, -1.0f, -1.0f },
          { -1.0f, -1.0f,  1.0f },
          {  1.0f, -1.0f,  1.0f }
        };
        TAGE::MEM::Ref<VertexBufferObject> _VBO;
        _VBO = VertexBufferObject::Create(skyboxVertices.data(), skyboxVertices.size() * sizeof(glm::vec3));
        BufferLayout layout = {
            { ShaderDataType::VEC3, "aPos" }
        };

        _VBO->SetLayout(layout);
        _SkyboxVAO->AddVertexBuffer(_VBO);
		_SkyboxVAO->SetCount(skyboxVertices.size());
	}
	Skybox::~Skybox()
	{
		_SkyboxShader.reset();
		_SkyboxTexture.reset();
		_SkyboxVAO.reset();
		LOG_INFO("Skybox destroyed");
	}

	void Skybox::Bind(const glm::mat4& view, const glm::mat4& projection) const
	{
		_SkyboxShader->Use();
		_SkyboxShader->SetUniform("view", glm::mat4(glm::mat3(view)));
		_SkyboxShader->SetUniform("projection", projection);

		_SkyboxTexture->Bind();
        RenderCommand::Disable(DEPTH_TEST);
        RenderCommand::DrawVertex(_SkyboxVAO);
		RenderCommand::Enable(DEPTH_TEST);
	}

	void Skybox::SetTexture(const char* path)
	{
		_SkyboxTexture->LoadCubemap(path);
		LOG_INFO("Skybox texture set to: {}", path);
	}
}