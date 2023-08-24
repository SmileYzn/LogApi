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
			this->m_Players[Auth].Auth = Auth;

			if (pszName)
			{
				if (pszName[0u] != '\0')
				{
					this->m_Players[Auth].Name = pszName;
				}
			}

			if (pszAddress)
			{
				if (pszAddress[0u] != '\0')
				{
					this->m_Players[Auth].Address = pszAddress;
				}
			}

			this->m_Players[Auth].UserId = g_engfuncs.pfnGetPlayerUserId(pEntity);

			this->m_Players[Auth].Team = pEntity->v.team;

			this->m_Players[Auth].Frags = pEntity->v.frags;

			this->m_Players[Auth].Deaths = 0;

			this->m_Players[Auth].GameTime = 0.0f;

			this->m_Players[Auth].ConnectTime = gpGlobals->time;
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
			if (this->m_Players.find(Auth) != this->m_Players.end())
			{
				this->m_Players.erase(Auth);
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
			this->m_Players[Auth].Auth = Auth;

			this->m_Players[Auth].Name = STRING(pEntity->v.netname);

			this->m_Players[Auth].UserId = g_engfuncs.pfnGetPlayerUserId(pEntity);

			auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

			if (Player)
			{
				this->m_Players[Auth].Team = static_cast<int>(Player->m_iTeam);

				this->m_Players[Auth].Frags = pEntity->v.frags;

				this->m_Players[Auth].Deaths = Player->m_iDeaths;

				if (this->m_Players[Auth].GameTime <= 0.0f)
				{
					if (Player->m_iTeam == UNASSIGNED)
					{
						if (!Player->IsBot())
						{
							gLogUtil.TeamInfo(Player->edict(), MAX_CLIENTS + TERRORIST + 1, "TERRORIST");
							gLogUtil.TeamInfo(Player->edict(), MAX_CLIENTS + CT + 1, "CT");
						}
					}
				}
			}

			if (this->m_Players[Auth].ConnectTime <= 0.0f)
			{
				this->m_Players[Auth].ConnectTime = gpGlobals->time;
			}

			this->m_Players[Auth].GameTime = (gpGlobals->time - this->m_Players[Auth].ConnectTime);
		}
	}
}
