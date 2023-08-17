#pragma once

// Client Print
constexpr auto PRINT_NOTIFY = 1;
constexpr auto PRINT_CONSOLE = 2;
constexpr auto PRINT_CHAT = 3;
constexpr auto PRINT_CENTER = 4;
constexpr auto PRINT_RADIO = 5;

class CLogUtil
{
public:
	cvar_t* CvarRegister(const char* Name, const char* Value);
	void ServerExecute(std::string Command);
	void ClientPrint(edict_t* pEntity, int msg_dest, const char* Format, ...);

private:
	std::map<std::string, cvar_t> m_Cvar;
};

extern CLogUtil gLogUtil;