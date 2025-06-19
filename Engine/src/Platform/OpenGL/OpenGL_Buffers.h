#pragma once
#include "TARE/Buffers/Buffers.h"

namespace TARE {
	class OpenGL_VertexBufferObject : public VertexBufferObject
	{
	public:
		OpenGL_VertexBufferObject(void* data, size_t size);
		virtual ~OpenGL_VertexBufferObject();

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void SetLayout(const BufferLayout& layout) override { _Layout = layout; }
		virtual const BufferLayout& GetLayout() const override { return _Layout; }
	private:
		uint _ID;
		BufferLayout _Layout;
	};

	class OpenGL_ElementBufferObject : public ElementBufferObject
	{
	public:
		OpenGL_ElementBufferObject(uint* indices, size_t count);
		virtual ~OpenGL_ElementBufferObject();

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual uint GetCount() const override { return _Count; }
	private:
		uint _ID;
		uint _Count;
	};
}