// Host-only test double for the lifecycle trace sink.
#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace LogMessageType { enum Type { kInfo }; }
struct OSystem {
	uint32_t now = 123;
	std::vector<std::string> messages;
	uint32_t getMillis() { return now; }
	void logMessage(LogMessageType::Type type, const char *message) {
		if (type == LogMessageType::kInfo)
			messages.emplace_back(message);
	}
};
extern OSystem *g_system;
