#include "tagepch.h"
#include "UUID.h"

namespace TAGE {
	static uint64_t GenerateRandomUUID() {
		static std::random_device rd;
		static std::mt19937_64 engine(rd());
		static std::uniform_int_distribution<uint64_t> dist;
		return dist(engine);
	}

	UUID::UUID() : _UUID(GenerateRandomUUID()) {}
	UUID::UUID(uint64_t uuid) : _UUID(uuid) {}

	UUID::UUID(const std::string& str) {
		_UUID = std::stoull(str, nullptr, 16);
	}

	std::string UUID::ToString() const {
		std::stringstream ss;
		ss << std::hex << std::setw(16) << std::setfill('0') << _UUID;
		return ss.str();
	}
}