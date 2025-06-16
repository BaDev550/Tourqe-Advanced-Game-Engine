#include "tagepch.h"
#include "Skybox.h"
#include "glm/glm.hpp"
#include "TAGE/Renderer/Shader/ShaderLibrary.h"
#include "TAGE/Renderer/Renderer.h"
#include "GLAD/glad.h"

namespace TAGE::Renderer {
    Skybox::Skybox()
    {
        _CubeMap = MEM::MakeRef<TextureCubeMap>(faces);
        _SkyBoxShader = ShaderLibrary::Get("SkyBoxShader");

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

        glGenVertexArrays(1, &_VAO);
        glGenBuffers(1, &_VBO);

        glBindVertexArray(_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(glm::vec3), skyboxVertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        glBindVertexArray(0);
    }
    void Skybox::DrawSkybox(const glm::mat4& view, const glm::mat4& projection)
    {
        glDepthFunc(GL_LEQUAL);
        _SkyBoxShader->Use();

        glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
        _SkyBoxShader->SetUniform("view", viewNoTranslation);
        _SkyBoxShader->SetUniform("projection", projection);

        _CubeMap->Bind();
        glBindVertexArray(_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }
}