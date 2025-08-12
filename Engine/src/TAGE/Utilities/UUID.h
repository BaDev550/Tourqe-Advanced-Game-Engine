#pragma once
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <functional>
#include "entt/entt.hpp"

namespace TAGE {
	class UUID {
	public:
		UUID();
		UUID(uint64 uuid);
		UUID(const std::string& str);
		UUID(entt::entity entityID) : _UUID(static_cast<uint64>(entityID)) {}

		std::string ToString() const;

		operator uint64() const { return _UUID; }
	private:
		uint64 _UUID;
	};
}

namespace std {
	template<>
	struct hash<TAGE::UUID> {
		std::size_t operator()(const TAGE::UUID& uuid) const noexcept {
			return std::hash<uint64>()(static_cast<uint64>(uuid));
		}
	};
}