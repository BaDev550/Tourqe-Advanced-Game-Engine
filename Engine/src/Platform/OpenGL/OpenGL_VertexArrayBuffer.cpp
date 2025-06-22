#include "tagepch.h"
#include "OpenGL_VertexArrayBuffer.h"
#include <GLAD/glad.h>

namespace TARE {
	static GLenum to_OpenGLType(ShaderDataType type) {
		switch (type)
		{
		case ShaderDataType::FLOAT:        return GL_FLOAT;
		case ShaderDataType::INT:          return GL_INT;
		case ShaderDataType::BOOL:         return GL_BOOL;
		case ShaderDataType::VEC2:         return GL_FLOAT;
		case ShaderDataType::VEC3:         return GL_FLOAT;
		case ShaderDataType::VEC4:         return GL_FLOAT;
		case ShaderDataType::MAT3:         return GL_FLOAT;
		case ShaderDataType::MAT4:         return GL_FLOAT;
		case ShaderDataType::SHORT3:       return GL_SHORT;
		case ShaderDataType::USHORT2:      return GL_UNSIGNED_SHORT;
		case ShaderDataType::BYTE3:        return GL_BYTE;
		case ShaderDataType::BYTE3_NORM:   return GL_BYTE;
		case ShaderDataType::UBYTE2_NORM:  return GL_UNSIGNED_BYTE;
		default:
			ASSERT_NOMSG(false);
			return 0;
		}
	}
	OpenGL_VertexArrayObject::OpenGL_VertexArrayObject() { glCreateVertexArrays(1, &_ID); }
	void OpenGL_VertexArrayObject::Bind() const { glBindVertexArray(_ID); }
	void OpenGL_VertexArrayObject::Unbind() const { glBindVertexArray(0); }

	void OpenGL_VertexArrayObject::AddVertexBuffer(const TAGE::MEM::Ref<VertexBufferObject>& vertexBuffer)
	{
		glBindVertexArray(_ID);
		vertexBuffer->Bind();

		const auto& layout = vertexBuffer->GetLayout();
		uint index = 0;

		for (const auto& element : layout) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				element.GetComponentCount(),
				to_OpenGLType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset
			);
			index++;
		}
		_VertexBuffers.push_back(vertexBuffer);
	}
	void OpenGL_VertexArrayObject::SetIndexBuffer(const TAGE::MEM::Ref<ElementBufferObject>& indexBuffer)
	{
		Bind();
		indexBuffer->Bind();
		_IndexBuffer = indexBuffer;
	}
}