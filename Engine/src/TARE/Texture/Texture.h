#pragma once
#include "TAGE/Utilities/Memory.h"
#include "TAGE/Utilities/Buffer.h"
#include "TAGE/Common/TDefines.h"

namespace TARE {
	enum class TextureType {
		Diffuse,
		Specular,
		Roughness,
		Normal,
		Metallic,
		AmbientOcclusion
	};

	struct TextureSpecs
	{
		int Width = 0;
		int Height = 0;
		int Channels = 0;
		uint InternalFormat;
		uint format;

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

		virtual uint GetID() const = 0;
		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;

		static TAGE::AssetType GetStaticType() { return TAGE::AssetType::Texture; }
		virtual TAGE::AssetType GetType() const { return GetStaticType(); };
	};
}
