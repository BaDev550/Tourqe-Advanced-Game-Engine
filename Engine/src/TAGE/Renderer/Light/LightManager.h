#pragma once
#include <vector>
#include "TAGE/Common/TDefines.h"
#include "TAGE/Common/TEnums.h"
#include "TAGE/Common/TTypes.h"
#include "TAGE/Utilities/Logger.h"

namespace TAGE::Renderer {
	class LightManager
	{
	public:
		LightManager() = default;
		~LightManager() { ClearLights(); }

		void AddLight(const Light& light) { 
			_Lights.push_back(light);
		}

		void RemoveLight(uint index) {
			if (index < _Lights.size()) {
				_Lights.erase(_Lights.begin() + index);
			}
		}
		void ClearLights() { _Lights.clear(); }

		std::vector<Light>& GetLights() { return _Lights; }
		uint GetLightCount() const { return static_cast<uint>(_Lights.size()); }
	private:
		std::vector<Light> _Lights;
	};
}