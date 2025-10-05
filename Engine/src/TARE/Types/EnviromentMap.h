#pragma once

#include "TAGE/AssetManager/Asset.h"
#include "TARE/Texture/TextureCube.h"
#include "TAGE/Utilities/Memory.h"

namespace TARE {
	class EnviromentMap : public TAGE::Asset
	{
	public:
		TAGE::MEM::Ref<TextureCube> RadianceMap;
		TAGE::MEM::Ref<TextureCube> IrradianceMap;
		TAGE::MEM::Ref<Texture2D> EquirectangularMap;

		EnviromentMap() = default;
		EnviromentMap(const TAGE::MEM::Ref<TextureCube>& radianceMap, const TAGE::MEM::Ref<TextureCube>& irradianceMap, const TAGE::MEM::Ref<Texture2D>& equirectangularMap)
			: RadianceMap(radianceMap), IrradianceMap(irradianceMap), EquirectangularMap(equirectangularMap) {
		}

		static TAGE::AssetType GetStaticType() { return TAGE::AssetType::EnviromentMap; }
		virtual TAGE::AssetType GetType() const { return GetStaticType(); };
	};
}