#include "tagepch.h"
#include "DeferredRendering.h"
#include "TAGE/Renderer/Shader/ShaderLibrary.h"

namespace TAGE::Renderer {
	DeferredRendering::DeferredRendering(int width, int height)
	{
        _DeferredShader = ShaderLibrary::Add("DeferredShader", "../Engine/shaders/Deferred/deferred_vertex.glsl", "../Engine/shaders/Deferred/deferred_fragment.glsl");
        _LightDeferredShader = ShaderLibrary::Add("LightDeferredShader", "../Engine/shaders/Deferred/deferred_lighting_vertex.glsl", "../Engine/shaders/Deferred/deferred_lighting_fragment.glsl");
		uint gPosition;
		uint gNormal;
		uint gAlbedoSpec;
        _Width = width;
        _Height = height;

		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

        // position color buffer
        glGenTextures(1, &gPosition);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
        _gPositionTex = MEM::MakeScope<Texture2D>(gPosition, width, height);

        // normal color buffer
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
        _gNormalTex = MEM::MakeScope<Texture2D>(gNormal, width, height);

        // color + specular color buffer
        glGenTextures(1, &gAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
        _gAlbedoTex = MEM::MakeScope<Texture2D>(gAlbedoSpec, width, height);

        GLuint gDepth;
        glGenRenderbuffers(1, &gDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, gDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepth);
        _gDepthTex = MEM::MakeScope<Texture2D>(gDepth, width, height);

        uint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        uint rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenFramebuffers(1, &_LightPassFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, _LightPassFBO);
        uint LightPass;

        glGenTextures(1, &LightPass);
        glBindTexture(GL_TEXTURE_2D, LightPass);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, LightPass, 0);
        _gLightPassTex = MEM::MakeScope<Texture2D>(LightPass, width, height);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "Light pass framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

    void DeferredRendering::BeginGeometryPass()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glViewport(0, 0, _Width, _Height);
        glClear(COLOR_DEPTH);
    }

    void DeferredRendering::RenderGeometryPass()
    {
        _DeferredShader->Use();
        _gPositionTex->Bind(0);
        _DeferredShader->SetUniform("gPosition", 0);
        _gNormalTex->Bind(1);
        _DeferredShader->SetUniform("gNormal", 1);
        _gAlbedoTex->Bind(2);
        _DeferredShader->SetUniform("gAlbedo", 2);

        _DeferredShader->SetUniform("roughness", 1.0f);
        _DeferredShader->SetUniform("metallic", 1.0f);
    }

    void DeferredRendering::RenderLightPass(const glm::vec3& lightPos, const glm::vec3& lightColor, const glm::vec3& cameraPos, const glm::mat4& lightMatrix)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _LightPassFBO);
        glViewport(0, 0, _Width, _Height);
        glClear(GL_COLOR_BUFFER_BIT);

        _LightDeferredShader->Use();
        _gPositionTex->Bind(10);
        _LightDeferredShader->SetUniform("u_gPos", 10);
        _gNormalTex->Bind(11);
        _LightDeferredShader->SetUniform("u_gNorm", 11);
        _gAlbedoTex->Bind(12);
        _LightDeferredShader->SetUniform("u_gAlb", 12);
        _gDepthTex->Bind(13);
        _LightDeferredShader->SetUniform("u_ShadowMap", 13);

        _LightDeferredShader->SetUniform("u_ScreenWidth", (float)_Width);
        _LightDeferredShader->SetUniform("u_ScreenHeight", (float)_Height);

        _LightDeferredShader->SetUniform("u_LightPos", lightPos);
        _LightDeferredShader->SetUniform("u_LightColor", lightColor);
        _LightDeferredShader->SetUniform("u_CameraPos", cameraPos);
        _LightDeferredShader->SetUniform("u_LightSpaceMatrix", lightMatrix);
    }
}