#pragma once

#include "TARE/Buffers/VertexArrayBuffer.h"

namespace TARE::OpenGL {
	class OpenGL_VertexArrayObject : public VertexArrayObject
	{
	public:
		OpenGL_VertexArrayObject();
		virtual ~OpenGL_VertexArrayObject() {};

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const TAGE::MEM::Ref<VertexBufferObject>& vertexBuffer) override;
		virtual void SetIndexBuffer(const  TAGE::MEM::Ref<ElementBufferObject>& indexBuffer) override;
		virtual TAGE::MEM::Ref<ElementBufferObject> GetIndexBuffer() const override { return _IndexBuffer ? _IndexBuffer : nullptr; }

		virtual void SetCount(size_t count) override { _Count = count; }
		virtual size_t GetVertexCount() const override { return _Count; }
		virtual void Destroy() override;
	private:
		std::vector<TAGE::MEM::Ref<VertexBufferObject>> _VertexBuffers;
		TAGE::MEM::Ref<ElementBufferObject> _IndexBuffer;
		uint _ID;
		size_t _Count = 0;
	};
}