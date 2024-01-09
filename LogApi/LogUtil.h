#pragma once

// Client Print
constexpr auto PRINT_NOTIFY = 1;
constexpr auto PRINT_CONSOLE = 2;
constexpr auto PRINT_CHAT = 3;
constexpr auto PRINT_CENTER = 4;
constexpr auto PRINT_RADIO = 5;

// Print SayText Colors
constexpr auto PRINT_TEAM_DEFAULT = 0;
constexpr auto PRINT_TEAM_GREY = -1;
constexpr auto PRINT_TEAM_RED = -2;
constexpr auto PRINT_TEAM_BLUE = -3;

class CLogUtil
{
public:
	cvar_t* CvarRegister(const char* Name, const char* Value);
	void ServerExecute(std::string Command);
	void ClientPrint(edict_t* pEntity, int msg_dest, const char* Format, ...);
	void TeamInfo(edict_t* pEntity, int playerIndex, const char* pszTeamName);
	void SayText(edict_t* pEntity, int Sender, const char* Format, ...);
	int ParseColors(char* Buffer);
	CBasePlayer* FindPlayer(std::string Target);
	unsigned short FixedUnsigned16(float value, float scale);
	short FixedSigned16(float value, float scale);
	hudtextparms_t HudParam(int red, int green, int blue, float x, float y, int effects, float fxtime, float holdtime, float fadeintime, float fadeouttime, int channel);
	void HudMessage(edict_t* pEntity, hudtextparms_t textparms, const char* Format, ...);
	void ShowMotd(edict_t* pEntity, char* Motd, int MotdLength);
	const char* GetAuthId(edict_t* pEntity);
	void ReplaceAll(std::string& String, const std::string& From, const std::string& To);
	std::vector<CBasePlayer*> GetPlayers();
private:
	std::map<std::string, cvar_t> m_Cvar;
};

extern CLogUtil gLogUtil;