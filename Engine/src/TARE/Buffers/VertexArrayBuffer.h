#pragma once
#include "Buffers.h"
#include "TAGE/Utilities/Memory.h"

namespace TARE {
	class VertexArrayObject
	{
	public:
		virtual ~VertexArrayObject() {};

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(const TAGE::MEM::Ref<VertexBufferObject>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const  TAGE::MEM::Ref<ElementBufferObject>& indexBuffer) = 0;
		virtual void SetCount(size_t count) = 0;
		virtual size_t GetCount() const = 0;
		virtual TAGE::MEM::Ref<ElementBufferObject> GetIndexBuffer() const = 0;
		virtual void Destroy() = 0;

		static TAGE::MEM::Ref<VertexArrayObject> Create();
	};
}