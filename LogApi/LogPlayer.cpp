#include "precompiled.h"
#include "LogPlayer.h"

CLogPlayer gLogPlayer;

void CLogPlayer::Connect(edict_t* pEntity, const char* pszName, const char* pszAddress)
{
	if (!FNullEnt(pEntity))
	{
		auto Auth = gLogUtil.GetAuthId(pEntity);

		if (Auth)
		{
			this->m_Data[Auth].Auth = Auth;

			if (pszName)
			{
				if (pszName[0u] != '\0')
				{
					this->m_Data[Auth].Name = pszName;
				}
			}

			if (pszAddress)
			{
				if (pszAddress[0u] != '\0')
				{
					this->m_Data[Auth].Address = pszAddress;
				}
			}

			this->m_Data[Auth].Team = pEntity->v.team;

			this->m_Data[Auth].Frags = pEntity->v.frags;

			this->m_Data[Auth].Deaths = 0;

			this->m_Data[Auth].GameTime = 0.0f;

			this->m_Data[Auth].ConnectTime = gpGlobals->time;
		}
	}
}

void CLogPlayer::Disconnect(edict_t* pEntity)
{
	if (!FNullEnt(pEntity))
	{
		auto Auth = gLogUtil.GetAuthId(pEntity);

		if (Auth)
		{
			if (this->m_Data.find(Auth) != this->m_Data.end())
			{
				this->m_Data.erase(Auth);
			}
		}
	}
}

void CLogPlayer::Update(edict_t* pEntity)
{
	if (!FNullEnt(pEntity))
	{
		auto Auth = gLogUtil.GetAuthId(pEntity);

		if (Auth)
		{
			this->m_Data[Auth].Auth = Auth;

			this->m_Data[Auth].Name = STRING(pEntity->v.netname);

			auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

			if (Player)
			{
				this->m_Data[Auth].Team = static_cast<int>(Player->m_iTeam);

				this->m_Data[Auth].Frags = pEntity->v.frags;

				this->m_Data[Auth].Deaths = Player->m_iDeaths;
			}

			this->m_Data[Auth].GameTime = (gpGlobals->time - this->m_Data[Auth].ConnectTime);
		}
	}
}

std::map<std::string, P_PLAYER_INFO> CLogPlayer::GetPlayers()
{
	return this->m_Data;
}
