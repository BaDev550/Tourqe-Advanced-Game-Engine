#pragma once

#include <GLAD/glad.h>
#include "TARE/Texture/Texture.h"

namespace TARE::OpenGL {
	static GLenum TextureFormatToOpenGLBaseFormat(TextureFormat format) {
		switch (format) {
		case TextureFormat::RGBA:        return GL_RGBA;
		case TextureFormat::RGB:       return GL_RGB;
		case TextureFormat::RED:      return GL_RED;
		case TextureFormat::RGBA8:      return GL_RGBA;
		case TextureFormat::RGBA16F:    return GL_RGBA;
		case TextureFormat::RGB16F:    return GL_RGB;
		case TextureFormat::R16F:       return GL_RED;
		case TextureFormat::R8:         return GL_RED;
		case TextureFormat::RED_INTEGER:     return GL_RED_INTEGER;
		case TextureFormat::DEPTH24STENCIL8: return GL_DEPTH_STENCIL;
		case TextureFormat::DEPTH32F:   return GL_DEPTH_COMPONENT;
		case TextureFormat::DEPTH24:    return GL_DEPTH_COMPONENT;
		case TextureFormat::DEPTH_ARRAY:return GL_DEPTH_COMPONENT;
		default:                              return 0;
		}
	}
	static GLenum TextureFormatToOpenGLInternalFormat(TextureFormat format) {
		switch (format) {
		case TextureFormat::RGBA:        return GL_RGBA;
		case TextureFormat::RGB:       return GL_RGB8;
		case TextureFormat::RED:      return GL_RED;
		case TextureFormat::RGBA8:      return GL_RGBA8;
		case TextureFormat::RGBA16F:    return GL_RGBA16F;
		case TextureFormat::RGB16F:    return GL_RGB16F;
		case TextureFormat::R16F:       return GL_R16F;
		case TextureFormat::R8:         return GL_R8;
		case TextureFormat::RED_INTEGER:     return GL_R32I;
		case TextureFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
		case TextureFormat::DEPTH32F:   return GL_DEPTH_COMPONENT32F;
		case TextureFormat::DEPTH24:    return GL_DEPTH_COMPONENT24;
		case TextureFormat::DEPTH_ARRAY:return GL_DEPTH_COMPONENT32F;
		default:                              return 0;
		}
	}
	static GLenum TextureDataTypeToOpenGLType(TextureDataType type) {
		switch (type) {
		case TextureDataType::UnsignedByte: return GL_UNSIGNED_BYTE;
		case TextureDataType::Float:        return GL_FLOAT;
		case TextureDataType::Int:          return GL_INT;
		default:                            return 0;
		}
	}
	static TextureFormat GetDataFormat(int channels) {
		switch (channels) {
		case 1: return TextureFormat::RED;
		case 3: return TextureFormat::RGB;
		case 4: return TextureFormat::RGBA;
		default:
			ASSERT(false, "Unsupported channel count");
			return TextureFormat::NONE;
		}
	}
	static TextureFormat GetInternalFormat(int channels) {
		switch (channels)
		{
		case 1: return TextureFormat::RED;
		case 3: return TextureFormat::RGB;
		case 4: return TextureFormat::RGBA8;
		default:
			ASSERT(false, "Unsupported channel count");
			return TextureFormat::NONE;
		}
	}
}