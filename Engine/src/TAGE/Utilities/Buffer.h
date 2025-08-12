#pragma once

#include <stdint.h>
#include <cstring>
#include "TAGE/Common/TDefines.h"

namespace TAGE {
	struct Buffer
	{
		uint8* Data = nullptr;
		uint64 Size = 0;

		Buffer() = default;

		Buffer(uint64 size)
		{
			Allocate(size);
		}

		Buffer(const void* data, uint64 size)
			: Data((uint8*)data), Size(size)
		{
		}

		Buffer(const Buffer&) = default;

		static Buffer Copy(Buffer other)
		{
			Buffer result(other.Size);
			memcpy(result.Data, other.Data, other.Size);
			return result;
		}

		void Allocate(uint64 size)
		{
			Release();

			Data = (uint8*)malloc(size);
			Size = size;
		}

		void Release()
		{
			free(Data);
			Data = nullptr;
			Size = 0;
		}

		template<typename T>
		T* As()
		{
			return (T*)Data;
		}

		operator bool() const
		{
			return (bool)Data;
		}

	};

	struct ScopedBuffer
	{
		ScopedBuffer(Buffer buffer)
			: _Buffer(buffer)
		{
		}

		ScopedBuffer(uint64 size)
			: _Buffer(size)
		{
		}

		~ScopedBuffer()
		{
			_Buffer.Release();
		}

		uint8* Data() { return _Buffer.Data; }
		uint64 Size() { return _Buffer.Size; }

		template<typename T>
		T* As()
		{
			return _Buffer.As<T>();
		}

		operator bool() const { return _Buffer; }
	private:
		Buffer _Buffer;
	};


}