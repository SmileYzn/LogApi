#include "precompiled.h"

IReGameApi *g_ReGameApi;
const ReGameFuncs_t *g_ReGameFuncs;
IReGameHookchains *g_ReGameHookchains;
CGameRules *g_pGameRules;

bool ReGameDLL_Init()
{
	auto szGameDLLModule = gpMetaUtilFuncs->pfnGetGameInfo(PLID, GINFO_DLL_FULLPATH);

	if (szGameDLLModule)
	{
		auto GameModule = Sys_GetModuleHandle(szGameDLLModule);

		if (GameModule)
		{
			auto ifaceFactory = Sys_GetFactory(GameModule);

			if (ifaceFactory)
			{
				int retCode = 0;

				g_ReGameApi = (IReGameApi *)ifaceFactory(VRE_GAMEDLL_API_VERSION, &retCode);

				if (g_ReGameApi)
				{
					if (g_ReGameApi->GetMajorVersion() == REGAMEDLL_API_VERSION_MAJOR)
					{
						if (g_ReGameApi->GetMinorVersion() >= REGAMEDLL_API_VERSION_MINOR)
						{
							if (g_ReGameApi->BGetICSEntity(CSENTITY_API_INTERFACE_VERSION))
							{
								if (g_ReGameApi->BGetIGameRules(GAMERULES_API_INTERFACE_VERSION))
								{
									g_ReGameFuncs = g_ReGameApi->GetFuncs();

									g_ReGameHookchains = g_ReGameApi->GetHookchains();

									if (g_ReGameHookchains)
									{
										g_ReGameHookchains->InstallGameRules()->registerHook(ReGameDLL_InstallGameRules);

                                        g_ReGameHookchains->InternalCommand()->registerHook(ReGameDLL_InternalCommand);
									}

									gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] ReGameDLL API Loaded: %d.%d", Plugin_info.logtag, REGAMEDLL_API_VERSION_MAJOR, REGAMEDLL_API_VERSION_MINOR);

									return true;
								}
							}
						}
					}
				}
			}
		}
	}

	gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] ReGameDLL API failed to load.", Plugin_info.logtag);

	return false;
}

bool ReGameDLL_Stop()
{
	if (g_ReGameHookchains)
	{
		g_ReGameHookchains->InstallGameRules()->unregisterHook(ReGameDLL_InstallGameRules);

        g_ReGameHookchains->InternalCommand()->unregisterHook(ReGameDLL_InternalCommand);
	}

	return true;
}

CGameRules *ReGameDLL_InstallGameRules(IReGameHook_InstallGameRules *chain)
{
	auto Result = chain->callNext();

	if (g_ReGameApi)
	{
		g_pGameRules = Result;
	}

	return Result;
}

void ReGameDLL_InternalCommand(IReGameHook_InternalCommand* chain, edict_t* pEntity, const char* pcmd, const char* parg1)
{
	if (pcmd)
	{
		if (pcmd[0u] != '\0')
		{
			if (Q_stricmp(pcmd, "menuselect") == 0)
			{
				if (parg1)
				{
					if (!FNullEnt(pEntity))
					{
						auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

						if (Player)
						{
							if (Player->m_iMenu == Menu_OFF)
							{
								if (gLogMenu[Player->entindex()].Handle(Player->entindex(), Q_atoi(parg1)))
								{
									// return;
								}
							}
						}
					}
				}
			}
		}
	}

	chain->callNext(pEntity, pcmd, parg1);
}
