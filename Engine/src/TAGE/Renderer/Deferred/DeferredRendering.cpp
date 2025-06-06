#include "tagepch.h"
#include "DeferredRendering.h"
#include "TAGE/Renderer/Shader/ShaderLibrary.h"

namespace TAGE::Renderer {
	DeferredRendering::DeferredRendering(int width, int height)
	{
        _DeferredShader = ShaderLibrary::Add("DeferredShader", "../Engine/shaders/Deferred/deferred_vertex.glsl", "../Engine/shaders/Deferred/deferred_fragment.glsl");
		uint gPosition;
		uint gNormal;
		uint gAlbedoSpec;

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
	}

    void DeferredRendering::BeginGeometryPass()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(COLOR_DEPTH);
    }

    void DeferredRendering::RenderGeometryPass()
    {
        _DeferredShader->Use();
        _gPositionTex->Bind(0);
        _DeferredShader->SetUniform("u_gPos", 0);
        _gNormalTex->Bind(1);
        _DeferredShader->SetUniform("u_gNorm", 1);
        _gAlbedoTex->Bind(2);
        _DeferredShader->SetUniform("u_gAlb", 2);
    }
}