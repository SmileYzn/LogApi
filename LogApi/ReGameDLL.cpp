#include "precompiled.h"

IReGameApi			*g_ReGameApi;
const ReGameFuncs_t	*g_ReGameFuncs;
IReGameHookchains	*g_ReGameHookchains;
CGameRules			*g_pGameRules;

bool ReGameDLL_Init()
{
	const char *szGameDLLModule = GET_GAME_INFO(PLID, GINFO_DLL_FULLPATH);

	if (!szGameDLLModule)
	{
		LOG_CONSOLE(PLID, "[%s] Failed to get GameDLL module", Plugin_info.logtag);
		return false;
	}

	CSysModule *gameModule = Sys_LoadModule(szGameDLLModule);

	if (!gameModule)
	{
		LOG_CONSOLE(PLID, "[%s] Failed to locate GameDLL module", Plugin_info.logtag);
		return false;
	}

	CreateInterfaceFn ifaceFactory = Sys_GetFactory(gameModule);

	if (!ifaceFactory)
	{
		LOG_CONSOLE(PLID, "[%s] Failed to locate interface factory in GameDLL module", Plugin_info.logtag);
		return false;
	}

	int retCode = 0;

	g_ReGameApi = (IReGameApi *)ifaceFactory(VRE_GAMEDLL_API_VERSION, &retCode);

	if (!g_ReGameApi)
	{
		LOG_CONSOLE(PLID, "[%s] Failed to locate retrieve rehlds api interface from GameDLL module, return code is %d", Plugin_info.logtag, retCode);
		return false;
	}

	int majorVersion = g_ReGameApi->GetMajorVersion();
	int minorVersion = g_ReGameApi->GetMinorVersion();

	if (majorVersion != REGAMEDLL_API_VERSION_MAJOR)
	{
		LOG_CONSOLE(PLID, "[%s] ReGameDLL API major version mismatch; expected %d, real %d", Plugin_info.logtag, REGAMEDLL_API_VERSION_MAJOR, majorVersion);

		if (majorVersion < REGAMEDLL_API_VERSION_MAJOR)
		{
			LOG_CONSOLE(PLID, "[%s] Please update the ReGameDLL up to a major version API >= %d", Plugin_info.logtag, REGAMEDLL_API_VERSION_MAJOR);
		}
		else if (majorVersion > REGAMEDLL_API_VERSION_MAJOR)
		{
			LOG_CONSOLE(PLID, "[%s] Please update the %s up to a major version API >= %d", Plugin_info.logtag, Plugin_info.logtag, majorVersion);
		}

		return false;
	}

	if (minorVersion < REGAMEDLL_API_VERSION_MINOR)
	{
		LOG_CONSOLE(PLID, "[%s] ReGameDLL API minor version mismatch; expected at least %d, real %d", Plugin_info.logtag, REGAMEDLL_API_VERSION_MINOR, minorVersion);
		LOG_CONSOLE(PLID, "[%s] Please update the ReGameDLL up to a minor version API >= %d", Plugin_info.logtag, REGAMEDLL_API_VERSION_MINOR);
		return false;
	}

	g_ReGameFuncs = g_ReGameApi->GetFuncs();

	g_ReGameHookchains = g_ReGameApi->GetHookchains();

	if (!g_ReGameApi->BGetICSEntity(CSENTITY_API_INTERFACE_VERSION))
	{
		LOG_CONSOLE(PLID, "[%s] Interface CCSEntity API version '%s' not found", Plugin_info.logtag, CSENTITY_API_INTERFACE_VERSION);
		return false;
	}

	g_ReGameHookchains->InstallGameRules()->registerHook(ReGameDLL_InstallGameRules);

	g_ReGameHookchains->InternalCommand()->registerHook(ReGameDLL_InternalCommand);

	return true;
}

bool ReGameDLL_Stop()
{
	g_ReGameHookchains->InstallGameRules()->unregisterHook(ReGameDLL_InstallGameRules);

	g_ReGameHookchains->InternalCommand()->unregisterHook(ReGameDLL_InternalCommand);

	return true;
}

CGameRules *ReGameDLL_InstallGameRules(IReGameHook_InstallGameRules *chain)
{
	auto gamerules = chain->callNext();

	if (!g_ReGameApi->BGetIGameRules(GAMERULES_API_INTERFACE_VERSION))
	{
		LOG_CONSOLE(PLID, "[%s] Interface GameRules API version '%s' not found", Plugin_info.logtag, GAMERULES_API_INTERFACE_VERSION);
	}
	else
	{
		g_pGameRules = gamerules;
	}
	
	return gamerules;
}

void ReGameDLL_InternalCommand(IReGameHook_InternalCommand* chain, edict_t* pEntity, const char* pcmd, const char* parg1)
{
	// If is not null
	if (pcmd)
	{
		// If string is not empty
		if (pcmd[0u] != '\0')
		{
			// If is menuselect command
			if (!Q_strcmp(pcmd, "menuselect"))
			{
				// If is not null
				if (parg1)
				{
					// Get player entity
					auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

					// if is not null
					if (Player)
					{
						// If native CS menu is not being displayed
						if (Player->m_iMenu == Menu_OFF)
						{
							// Handle menu
							if (gLogMenu[Player->entindex()].Handle(Player->entindex(), atoi(parg1)))
							{
								// Return, block original function call
								return;
							}
						}
					}
				}
			}
		}
	}

	// Call original function
	chain->callNext(pEntity, pcmd, parg1);
}