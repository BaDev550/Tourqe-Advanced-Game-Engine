#pragma once
#include "TARE/Buffers/VertexArrayBuffer.h"

namespace TARE {
	class OpenGL_VertexArrayObject : public VertexArrayObject
	{
	public:
		OpenGL_VertexArrayObject();
		virtual ~OpenGL_VertexArrayObject() {};

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const TAGE::MEM::Ref<VertexBufferObject>& vertexBuffer) override;
		virtual void SetIndexBuffer(const  TAGE::MEM::Ref<ElementBufferObject>& indexBuffer) override;
		virtual TAGE::MEM::Ref<ElementBufferObject> GetIndexBuffer() const override { return _IndexBuffer; }

		virtual void SetCount(size_t count) override { _Count = count; }
		virtual size_t GetCount() const override { return _Count; }
	private:
		std::vector<TAGE::MEM::Ref<VertexBufferObject>> _VertexBuffers;
		std::shared_ptr<ElementBufferObject> _IndexBuffer;
		uint32_t _ID;
		size_t _Count = 0;
	};
}