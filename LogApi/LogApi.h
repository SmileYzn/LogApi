#pragma once

// Log API Settings
constexpr auto LOG_API_FILE_EVENTS = "cstrike/addons/logapi/events.json";

// Log API temporary config
constexpr auto LOG_API_TEMP_FILE = "cstrike/addons/logapi/temp.cfg";

class CLogApi
{
public:
	// On Server Activate Event
	void ServerActivate();

	// Check if event is enabled
	bool EventEnabled(const char* EventName);

	// Send Event
	void SendEvent(int EventIndex, nlohmann::ordered_json EventData);

	// Callback Result
	static void CallbackResult(CURL* ch, size_t Size, const char* Memory, int EventIndex);

	// Parse Event Result
	void EventResult(int EventIndex, nlohmann::ordered_json Result);

private:
	// Console variables
	cvar_t* m_log_api_on;
	cvar_t* m_log_api_address;
	cvar_t* m_log_api_timeout;
	cvar_t* m_log_api_bearer;

	// Events
	std::map<std::string, bool> m_Events;
};

extern CLogApi gLogApi;