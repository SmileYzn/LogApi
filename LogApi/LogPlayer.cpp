#include "precompiled.h"

CLogPlayer gLogPlayer;

void CLogPlayer::Connect(edict_t *pEdict, const char *pszName,
                         const char *pszAddress) {
  if (!FNullEnt(pEdict)) {
    auto Auth = gLogUtil.GetAuthId(pEdict);

    if (Auth) {
      this->m_Players[Auth].Auth = Auth;

      if (pszName) {
        if (pszName[0u] != '\0') {
          this->m_Players[Auth].Name = pszName;
        }
      }

      if (pszAddress) {
        if (pszAddress[0u] != '\0') {
          this->m_Players[Auth].Address = pszAddress;
        }
      }

      this->m_Players[Auth].EntityId = ENTINDEX(pEdict);

      this->m_Players[Auth].UserId = g_engfuncs.pfnGetPlayerUserId(pEdict);

      this->m_Players[Auth].Team = pEdict->v.team;

      this->m_Players[Auth].Frags = pEdict->v.frags;

      this->m_Players[Auth].Deaths = 0;

      this->m_Players[Auth].Ping = 0;

      this->m_Players[Auth].GameTime = 0.0f;

      this->m_Players[Auth].ConnectTime = gpGlobals->time;
    }
  }
}

void CLogPlayer::Disconnect(edict_t *pEdict) {
  if (!FNullEnt(pEdict)) {
    auto Auth = gLogUtil.GetAuthId(pEdict);

    if (Auth) {
      if (this->m_Players.find(Auth) != this->m_Players.end()) {
        this->m_Players.erase(Auth);
      }
    }
  }
}

void CLogPlayer::Update(edict_t *pEdict) {
  if (FNullEnt(pEdict) || !pEdict->pvPrivateData)
    return;

  auto Auth = gLogUtil.GetAuthId(pEdict);

  if (!Auth || Auth[0u] == '\0')
    return;

  for (auto it = this->m_Players.begin(); it != this->m_Players.end();) {
    if (it->second.EntityId == ENTINDEX(pEdict) && it->first != Auth) {
      it = this->m_Players.erase(it);
    } else {
      ++it;
    }
  }

  this->m_Players[Auth].EntityId = ENTINDEX(pEdict);

  this->m_Players[Auth].Auth = Auth;

  auto NetName = STRING(pEdict->v.netname);
  this->m_Players[Auth].Name = NetName ? NetName : "";

  this->m_Players[Auth].UserId = g_engfuncs.pfnGetPlayerUserId(pEdict);

  auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEdict));

  if (Player) {
    this->m_Players[Auth].Team = static_cast<int>(Player->m_iTeam);

    this->m_Players[Auth].Frags = pEdict->v.frags;

    this->m_Players[Auth].Deaths = Player->m_iDeaths;

    int ping = 0, loss = 0;
    g_engfuncs.pfnGetPlayerStats(pEdict, &ping, &loss);
    this->m_Players[Auth].Ping = ping;

    if (this->m_Players[Auth].GameTime <= 0.0f) {
      if (Player->m_iTeam == UNASSIGNED) {
        if (!Player->IsBot()) {
          gLogUtil.TeamInfo(Player->edict(), MAX_CLIENTS + TERRORIST + 1,
                            "TERRORIST");
          gLogUtil.TeamInfo(Player->edict(), MAX_CLIENTS + CT + 1, "CT");
        }
      }
    }
  }

  if (this->m_Players[Auth].ConnectTime <= 0.0f) {
    this->m_Players[Auth].ConnectTime = gpGlobals->time;
  }

  this->m_Players[Auth].GameTime =
      (gpGlobals->time - this->m_Players[Auth].ConnectTime);
}

std::map<std::string, P_PLAYER_INFO> CLogPlayer::GetPlayers() {
  return this->m_Players;
}

LP_PLAYER_INFO CLogPlayer::GetPlayer(std::string Auth) {
  if (this->m_Players.find(Auth) != this->m_Players.end()) {
    return &this->m_Players[Auth];
  }

  return nullptr;
}

nlohmann::ordered_json CLogPlayer::GetPlayerJson(edict_t *pEdict) {
  nlohmann::ordered_json PlayerJson;

  // Reject null, invalid, or partially-destroyed edicts
  if (FNullEnt(pEdict) || !pEdict->pvPrivateData)
    return PlayerJson;

  // Copy the auth string immediately — pfnGetPlayerAuthId returns an internal
  // engine pointer that can be invalidated by subsequent engine calls
  const char* rawAuth = g_engfuncs.pfnGetPlayerAuthId(pEdict);
  if (!rawAuth || rawAuth[0u] == '\0')
    return PlayerJson;

  std::string AuthStr(rawAuth);

  try {
    auto Player = this->GetPlayer(AuthStr);

    if (Player != nullptr) {
      PlayerJson = {{"EntityId",    Player->EntityId},
                    {"Auth",        Player->Auth},
                    {"Name",        Player->Name},
                    {"Address",     Player->Address},
                    {"UserId",      Player->UserId},
                    {"Team",        Player->Team},
                    {"Frags",       Player->Frags},
                    {"Deaths",      Player->Deaths},
                    {"Ping",        Player->Ping},
                    {"GameTime",    Player->GameTime},
                    {"ConnectTime", Player->ConnectTime}};
    }
  } catch (const std::exception &e) {
    LOG_CONSOLE(PLID, "[%s] Exception building player JSON: %s", __func__, e.what());
    PlayerJson = nlohmann::ordered_json{};
  }

  return PlayerJson;
}
