#include "precompiled.h"

#pragma region METAMOD

plugin_info_t Plugin_info = {
    META_INTERFACE_VERSION,
    "Log API",
    "0.0.2",
    __DATE__,
    "SmileY",
    "https://github.com/SmileYzn/logapi",
    "LOG",
    PT_STARTUP,
    PT_ANYTIME,
};

// Global Pointers (Dynamic Allocation)
CLogApi* g_pLogApi = nullptr;
CLogCurl* g_pLogCurl = nullptr;

enginefuncs_t g_engfuncs;
globalvars_t *gpGlobals;
meta_globals_t *gpMetaGlobals;
gamedll_funcs_t *gpGamedllFuncs;
mutil_funcs_t *gpMetaUtilFuncs;
META_FUNCTIONS gMetaFunctionTable;

C_DLLEXPORT void WINAPI GiveFnptrsToDll(enginefuncs_t *pengfuncsFromEngine,
                                        globalvars_t *pGlobals) {
    memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
    gpGlobals = pGlobals;
} // ✅ Closes GiveFnptrsToDll

C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME now, META_FUNCTIONS *pFunctionTable,
                            meta_globals_t *pMGlobals,
                            gamedll_funcs_t *pGamedllFuncs) {
    gpMetaGlobals = pMGlobals;
    gpGamedllFuncs = pGamedllFuncs;

    memset(&gMetaFunctionTable, 0, sizeof(META_FUNCTIONS));

    gMetaFunctionTable.pfnGetEntityAPI2 = GetEntityAPI2;
    gMetaFunctionTable.pfnGetEntityAPI2_Post = GetEntityAPI2_Post;
    gMetaFunctionTable.pfnGetEngineFunctions = GetEngineFunctions;
    gMetaFunctionTable.pfnGetEngineFunctions_Post = GetEngineFunctions_Post;
    gMetaFunctionTable.pfnGetNewDLLFunctions = GetNewDLLFunctions;
    gMetaFunctionTable.pfnGetNewDLLFunctions_Post = GetNewDLLFunctions_Post;

    memcpy(pFunctionTable, &gMetaFunctionTable, sizeof(META_FUNCTIONS));

    // Dynamic Initialization (Prevents SIGSEGV on Linux)
    if (g_pLogApi == nullptr) {
        g_pLogApi = new CLogApi();
    }

    if (g_pLogCurl == nullptr) {
        g_pLogCurl = new CLogCurl();
    }

    ReAPI_Init();
    ReGameDLL_Init();

    return TRUE;
} // ✅ Closes Meta_Attach

C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME now, PL_UNLOAD_REASON reason) {
    ReAPI_Stop();
    ReGameDLL_Stop();

    // Memory Cleanup
    if (g_pLogApi != nullptr) {
        delete g_pLogApi;
        g_pLogApi = nullptr;
    }

    if (g_pLogCurl != nullptr) {
        delete g_pLogCurl;
        g_pLogCurl = nullptr;
    }

    return TRUE;
} // ✅ Closes Meta_Detach

C_DLLEXPORT int Meta_Query(char *interfaceVersion, plugin_info_t **pPlugInfo,
                           mutil_funcs_t *pMetaUtilFuncs) {
    *pPlugInfo = &Plugin_info;
    gpMetaUtilFuncs = pMetaUtilFuncs;

    return TRUE;
} // ✅ Closes Meta_Query

#pragma endregion // ✅ Closes METAMOD region
