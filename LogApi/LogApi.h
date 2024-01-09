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

	// On Server Frame
	void ServerFrame();

	// Check if event is enabled
	bool EventEnabled(const char* EventName);

	// Send Event
	void SendEvent(int EventIndex, nlohmann::ordered_json EventData);

	// Callback Result
	static void CallbackResult(CURL* ch, size_t Size, const char* Memory, int EventIndex);

	// Parse Event Result
	void EventResult(int EventIndex, nlohmann::ordered_json Result);

	// Server Execute
	void ServerExecute(int EventIndex, nlohmann::ordered_json Data);

	// Server Meu
	void ServerMenu(int EventIndex, nlohmann::ordered_json Data);

	// Open  Menu
	void Menu(int EntityIndex, std::string Title, bool Exit, std::string Callback, nlohmann::ordered_json Items);

	// Menu Handle
	static void MenuHandle(int EntityIndex, std::string Callback, P_MENU_ITEM Item);

	// Get Server info
	nlohmann::ordered_json GetServerInfo();

private:
	// Console variables
	cvar_t* m_log_api_on;
	cvar_t* m_log_api_address;
	cvar_t* m_log_api_timeout;
	cvar_t* m_log_api_bearer;
	cvar_t* log_api_delay;

	// Events
	std::map<std::string, bool> m_Events;

	// Frame time
	float m_FrameTime = 0.0f;
};

extern CLogApi gLogApi;