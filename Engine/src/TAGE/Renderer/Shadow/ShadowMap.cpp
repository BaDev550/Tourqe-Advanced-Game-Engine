#include "tagepch.h"
#include "ShadowMap.h"
#include <GLAD/glad.h>

namespace TAGE::Renderer {
    ShadowMap::ShadowMap(uint width, uint height) {
        _Width = width;
        _Height = height;

        glCreateFramebuffers(1, &_Framebuffer);

        glCreateTextures(GL_TEXTURE_2D, 1, &_DepthMap); 
        glTextureStorage2D(_DepthMap, 1, GL_DEPTH_COMPONENT32F, width, height);
        glTextureParameteri(_DepthMap, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(_DepthMap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(_DepthMap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(_DepthMap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTextureParameteri(_DepthMap, GL_TEXTURE_COMPARE_MODE, GL_NONE);

        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTextureParameterfv(_DepthMap, GL_TEXTURE_BORDER_COLOR, borderColor);

        glNamedFramebufferTexture(_Framebuffer, GL_DEPTH_ATTACHMENT, _DepthMap, 0);
        glNamedFramebufferDrawBuffer(_Framebuffer, GL_NONE);
        glNamedFramebufferReadBuffer(_Framebuffer, GL_NONE);

        GLenum status = glCheckNamedFramebufferStatus(_Framebuffer, GL_FRAMEBUFFER);
        ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void ShadowMap::BindDepthTexture(int slot)
	{
        glBindTextureUnit(slot, _DepthMap);
	}
    void ShadowMap::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glCullFace(GL_BACK);
    }
    void ShadowMap::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _Framebuffer);
        glViewport(0, 0, _Width, _Height);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
    }
}