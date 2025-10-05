#include "tagepch.h"
#include "OpenGL_TextureCube.h"
#include "OpenGL_TextureUtils.h"

#include <GLAD/glad.h>

namespace TARE::OpenGL {
    OpenGL_TextureCube::OpenGL_TextureCube(const TextureSpecs& specification, TAGE::Buffer Data)
        : _Specification(specification), _ID(0)
    {
        glGenTextures(1, &_ID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _ID);

        GLenum internalFormat = TextureFormatToOpenGLInternalFormat(specification.InternalFormat);
        GLenum baseFormat = TextureFormatToOpenGLBaseFormat(specification.format);
        GLenum type = TextureDataTypeToOpenGLType(_Specification.DataType);
        
        for (uint i = 0; i < 6; ++i)
        {
            void* dataPtr = Data ? Data.Data : nullptr;
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
				internalFormat,
                specification.Width,
                specification.Height,
                0,
                baseFormat,
                type,
                dataPtr
            );
        }
        glTextureParameteri(_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(_ID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        if (specification.CreateMipmaps)
            glGenerateTextureMipmap(_ID);

        if (Data.Data)
            Data.Release();
	}
	void OpenGL_TextureCube::Bind(uint8 slot) const
	{
        if (_ID == 0)
			return;

        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _ID);
	}
	void OpenGL_TextureCube::Unbind() const
	{
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
    void OpenGL_TextureCube::GenerateMipmap() const
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, _ID);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
}