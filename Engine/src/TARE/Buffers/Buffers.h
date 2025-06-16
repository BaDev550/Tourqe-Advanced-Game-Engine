#pragma once
#include "TAGE/Common/TEnums.h"

namespace TARE {
	static uint ShaderDataTypeSize(ShaderDataType type) {
		switch (type)
		{
		case ShaderDataType::NONE: return 0;
		case ShaderDataType::FLOAT:return 4;
		case ShaderDataType::INT:  return 4;
		case ShaderDataType::BOOL: return 4;
		case ShaderDataType::VEC2: return 4*2;
		case ShaderDataType::VEC3: return 4*3;
		case ShaderDataType::VEC4: return 4*4;
		case ShaderDataType::MAT3: return 4*3*3;
		case ShaderDataType::MAT4: return 4*4*4;
		default:
			ASSERT_NOMSG(false);
			return 0;
		}
	}

	struct BufferElement {
		const char* Name;
		ShaderDataType Type;
		uint16 Size;
		uint16 Offset;
		bool Normalized;

		uint GetComponentCount() const {
			switch (Type)
			{
			case ShaderDataType::NONE: return 0;
			case ShaderDataType::FLOAT:return 1;
			case ShaderDataType::INT:  return 1;
			case ShaderDataType::BOOL: return 1;
			case ShaderDataType::VEC2: return 2;
			case ShaderDataType::VEC3: return 3;
			case ShaderDataType::VEC4: return 4;
			case ShaderDataType::MAT3: return 3*3;
			case ShaderDataType::MAT4: return 4*4;
			default:
				ASSERT_NOMSG(false);
				break;
			}
		}

		BufferElement() {}
		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false) : Name(name.c_str()), Type(type), Normalized(normalized) {}
	};

	class BufferLayout {
	public:
		BufferLayout() {}
		BufferLayout(const std::initializer_list<BufferElement>& elements) 
			: _Elements(elements)
		{
			CalculateOffsetAndStride();
		}
		inline const std::vector<BufferElement>& GetElements() const { return _Elements; }
		inline uint32_t GetStride() const { return _Stride; }

		std::vector<BufferElement>::iterator begin() { return _Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return _Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return _Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return _Elements.end(); }
	private:
		void CalculateOffsetAndStride() {
			uint16 offset = 0;
			_Stride = 0;
			for (auto& element : _Elements) {
				element.Offset = offset;
				offset += element.Size;
				_Stride += element.Size;
			}
		}
		std::vector<BufferElement> _Elements;
		uint16 _Stride = 0;
	};

	class VertexBufferObject
	{
	public:
		virtual ~VertexBufferObject() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout() const = 0;

		static VertexBufferObject* Create(float* vertices, uint16 size);
	};

	class ElementBufferObject
	{
	public:
		virtual ~ElementBufferObject() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual uint GetCount() const = 0;

		static ElementBufferObject* Create(uint* indices, uint size);
	};
}