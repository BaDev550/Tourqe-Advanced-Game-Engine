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

	private:
		std::vector<TAGE::MEM::Ref<VertexBufferObject>> _VertexBuffers;
		std::shared_ptr<ElementBufferObject> _IndexBuffer;
		uint32_t _ID;
	};
}