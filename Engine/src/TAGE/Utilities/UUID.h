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
		UUID(uint64_t uuid);
		UUID(const std::string& str);
		UUID(entt::entity entityID) : _UUID(static_cast<uint64_t>(entityID)) {}

		std::string ToString() const;

		operator uint64_t() const { return _UUID; }
		operator entt::entity() const { return static_cast<entt::entity>(_UUID); }
		bool operator==(const UUID& other) const { return _UUID == other._UUID; }
		bool operator!=(const UUID& other) const { return _UUID != other._UUID; }

	private:
		uint64_t _UUID;
	};
}

namespace std {
	template<>
	struct hash<TAGE::UUID> {
		std::size_t operator()(const TAGE::UUID& uuid) const noexcept {
			return std::hash<uint64_t>()(static_cast<uint64_t>(uuid));
		}
	};
}