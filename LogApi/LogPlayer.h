#pragma once

typedef struct S_PLAYER_INFO
{
	std::string Auth;
	std::string Name;
	std::string Address;
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
	void Connect(edict_t* pEntity, const char* pszName, const char* pszAddress);
	void Disconnect(edict_t* pEntity);
	void Update(edict_t* pEntity);
	std::map<std::string, P_PLAYER_INFO> m_Players;
};

extern CLogPlayer gLogPlayer;