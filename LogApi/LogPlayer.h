#pragma once

typedef struct S_PLAYER_INFO
{
	std::string Auth;
	std::string Name;
	std::string Address;
	int EntityId;
	int UserId;
	int Team;
	int Frags;
	int Deaths;
	float GameTime;
	float ConnectTime;
} P_PLAYER_INFO, *LP_PLAYER_INFO;

class CLogPlayer
{
public:
	void Connect(edict_t* pEdict, const char* pszName, const char* pszAddress);
	void Disconnect(edict_t* pEdict);
	void Update(edict_t* pEdict);
	std::map<std::string, P_PLAYER_INFO> GetPlayers();
	LP_PLAYER_INFO GetPlayer(std::string Auth);
	nlohmann::ordered_json GetPlayerJson(edict_t* pEdict);

private:
	std::map<std::string, P_PLAYER_INFO> m_Players;
};

extern CLogPlayer gLogPlayer;