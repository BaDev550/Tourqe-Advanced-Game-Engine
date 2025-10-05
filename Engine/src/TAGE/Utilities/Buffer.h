#pragma once

#include <stdint.h>
#include <cstring>
#include "TAGE/Common/TDefines.h"

namespace TAGE {
	struct Buffer
	{
		void* Data = nullptr;
		uint64_t Size = 0;

		Buffer() = default;

		Buffer(const void* data, uint64_t size = 0)
			: Data((void*)data), Size(size) {
		}

		static Buffer Copy(const Buffer& other)
		{
			Buffer buffer;
			buffer.Allocate(other.Size);
			memcpy(buffer.Data, other.Data, other.Size);
			return buffer;
		}

		static Buffer Copy(const void* data, uint64_t size)
		{
			Buffer buffer;
			buffer.Allocate(size);
			memcpy(buffer.Data, data, size);
			return buffer;
		}

		void Allocate(uint64_t size)
		{
			delete[](uint8*)Data;
			Data = nullptr;
			Size = size;

			if (size == 0)
				return;

			Data = new uint8[size];
		}

		void Release()
		{
			delete[](uint8*)Data;
			Data = nullptr;
			Size = 0;
		}

		template<typename T>
		T& Read(uint64_t offset = 0)
		{
			return *(T*)((uint8*)Data + offset);
		}

		template<typename T>
		const T& Read(uint64_t offset = 0) const
		{
			return *(T*)((uint8*)Data + offset);
		}

		uint8* ReadBytes(uint64_t size, uint64_t offset) const
		{
			ASSERT(offset + size <= Size, "Buffer overflow!");
			uint8* buffer = new uint8[size];
			memcpy(buffer, (uint8*)Data + offset, size);
			return buffer;
		}

		void Write(const void* data, uint64_t size, uint64_t offset = 0)
		{
			ASSERT(offset + size <= Size, "Buffer overflow!");
			memcpy((uint8*)Data + offset, data, size);
		}

		operator bool() const
		{
			return (bool)Data;
		}

		uint8& operator[](int index)
		{
			return ((uint8*)Data)[index];
		}

		uint8 operator[](int index) const
		{
			return ((uint8*)Data)[index];
		}

		template<typename T>
		T* As() const
		{
			return (T*)Data;
		}

		inline uint64_t GetSize() const { return Size; }
	};
}