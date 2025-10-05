#pragma once
#include "TAGE/Utilities/Memory.h"
#include "TAGE/Utilities/Buffer.h"
#include "TAGE/Common/TDefines.h"

namespace TARE {
	enum class TextureFormat : uint8 {
		NONE = 0,
		RGBA,
		RGB,
		RED,
		RGBA8,
		RGB16F,
		RGBA16F,
		R16F,
		R8,
		RED_INTEGER,
		DEPTH24STENCIL8,
		DEPTH32F,
		DEPTH24,
		DEPTH_ARRAY,
		CUBE_MAP
	};

	enum class TextureDataType : uint8 {
		None = 0,
		UnsignedByte,
		Float,
		Int
	};

	enum class TextureType : uint8 {
		None = 0,
		Texture2D,
		TextureCube
	};

	struct TextureSpecs
	{
		int Width = 0;
		int Height = 0;
		int Channels = 0;
		TextureFormat InternalFormat;
		TextureFormat format;
		TextureDataType DataType = TextureDataType::UnsignedByte;

		bool CreateMipmaps = true;

		bool IsValid() const {
			return Width > 0 && Height > 0;
		}
	};

	class Texture : public TAGE::Asset
	{
	public:
		virtual ~Texture() = default;
		virtual void Bind(uint8 slot = 0) const = 0;
		virtual void Unbind() const = 0;
		virtual void GenerateMipmap() const = 0;

		virtual uint GetID() const = 0;
		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;
		virtual TextureType GetTextureType() const = 0;

		static TAGE::AssetType GetStaticType() { return TAGE::AssetType::Texture; }
		virtual TAGE::AssetType GetType() const { return GetStaticType(); };
	};
}
