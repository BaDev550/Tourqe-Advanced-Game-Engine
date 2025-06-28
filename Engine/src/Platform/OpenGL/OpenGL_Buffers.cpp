#include "tagepch.h"
#include "OpenGL_Buffers.h"
#include <GLAD/glad.h>

namespace TARE {
	OpenGL_VertexBufferObject::OpenGL_VertexBufferObject(void* data, size_t size)
	{
		if (data) {
			glCreateBuffers(1, &_ID);
			glBindBuffer(GL_ARRAY_BUFFER, _ID);
			glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		}
		else {
			glGenBuffers(1, &_ID);
			glBindBuffer(GL_ARRAY_BUFFER, _ID);
			glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		}

		_Size = size;
	}
	OpenGL_VertexBufferObject::~OpenGL_VertexBufferObject()
	{
		glDeleteBuffers(1, &_ID);
	}
	void OpenGL_VertexBufferObject::Bind() const { glBindBuffer(GL_ARRAY_BUFFER, _ID); }
	void OpenGL_VertexBufferObject::Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

	void OpenGL_VertexBufferObject::SetData(void* data, size_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, _ID);

		if (size > _Size) {
			glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
			_Size = size;
		}
		else {
			glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
		}
	}

	OpenGL_ElementBufferObject::OpenGL_ElementBufferObject(uint* indices, size_t count)
		: _Count(count)
	{
		glCreateBuffers(1, &_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), indices, GL_STATIC_DRAW);
	}
	OpenGL_ElementBufferObject::~OpenGL_ElementBufferObject()
	{
		glDeleteBuffers(1, &_ID);
	}
	void OpenGL_ElementBufferObject::Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ID); }
	void OpenGL_ElementBufferObject::Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
}