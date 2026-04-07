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

// Global Pointers
CLogApi* g_pLogApi = nullptr;
CLogCurl* g_pLogCurl = nullptr;

enginefuncs_t g_engfuncs;
globalvars_t *gpGlobals;
meta_globals_t *gpMetaGlobals;
gamedll_funcs_t *gpGamedllFuncs;
mutil_funcs_t *gpMetaUtilFuncs;
META_FUNCTIONS gMetaFunctionTable;

// ✅ Function Declarations (Required for Windows Linker)
C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion);
C_DLLEXPORT int GetEntityAPI2_Post(DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion);
C_DLLEXPORT int GetEngineFunctions(enginefuncs_t *pengfuncsFromEngine, int *interfaceVersion);
C_DLLEXPORT int GetEngineFunctions_Post(enginefuncs_t *pengfuncsFromEngine, int *interfaceVersion);
C_DLLEXPORT int GetNewDLLFunctions(NEW_DLL_FUNCTIONS *pNewFunctionTable, int *interfaceVersion);
C_DLLEXPORT int GetNewDLLFunctions_Post(NEW_DLL_FUNCTIONS *pNewFunctionTable, int *interfaceVersion);

// ✅ Function Definitions
DLL_FUNCTIONS g_DLL_FunctionTable_Pre;
DLL_FUNCTIONS g_DLL_FunctionTable_Post;
NEW_DLL_FUNCTIONS g_DLL_NewFunctionTable_Pre;
NEW_DLL_FUNCTIONS g_DLL_NewFunctionTable_Post;
enginefuncs_t g_ENGINE_FunctionTable_Pre;
enginefuncs_t g_ENGINE_FunctionTable_Post;

C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion) {
    memset(&g_DLL_FunctionTable_Pre, 0, sizeof(DLL_FUNCTIONS));
    memcpy(pFunctionTable, &g_DLL_FunctionTable_Pre, sizeof(DLL_FUNCTIONS));
    return 1;
}

C_DLLEXPORT int GetEntityAPI2_Post(DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion) {
    memset(&g_DLL_FunctionTable_Post, 0, sizeof(DLL_FUNCTIONS));
    g_DLL_FunctionTable_Post.pfnServerActivate = DLL_POST_ServerActivate;
    g_DLL_FunctionTable_Post.pfnServerDeactivate = DLL_POST_ServerDeactivate;
    g_DLL_FunctionTable_Post.pfnStartFrame = DLL_POST_StartFrame;
    g_DLL_FunctionTable_Post.pfnClientConnect = DLL_POST_ClientConnect;
    g_DLL_FunctionTable_Post.pfnClientPutInServer = DLL_POST_ClientPutInServer;
    g_DLL_FunctionTable_Post.pfnClientKill = DLL_POST_ClientKill;
    g_DLL_FunctionTable_Post.pfnClientUserInfoChanged = DLL_POST_ClientUserInfoChanged;
    g_DLL_FunctionTable_Post.pfnClientCommand = DLL_POST_ClientCommand;
    memcpy(pFunctionTable, &g_DLL_FunctionTable_Post, sizeof(DLL_FUNCTIONS));
    return 1;
}

C_DLLEXPORT int GetEngineFunctions(enginefuncs_t *pengfuncsFromEngine, int *interfaceVersion) {
    memset(&g_ENGINE_FunctionTable_Pre, 0, sizeof(enginefuncs_t));
    memcpy(pengfuncsFromEngine, &g_ENGINE_FunctionTable_Pre, sizeof(enginefuncs_t));
    return 1;
}

C_DLLEXPORT int GetEngineFunctions_Post(enginefuncs_t *pengfuncsFromEngine, int *interfaceVersion) {
    memset(&g_ENGINE_FunctionTable_Post, 0, sizeof(enginefuncs_t));
    g_ENGINE_FunctionTable_Post.pfnAlertMessage = ENGINE_POST_AlertMessage;
    memcpy(pengfuncsFromEngine, &g_ENGINE_FunctionTable_Post, sizeof(enginefuncs_t));
    return 1;
}

C_DLLEXPORT int GetNewDLLFunctions(NEW_DLL_FUNCTIONS *pNewFunctionTable, int *interfaceVersion) {
    memset(&g_DLL_NewFunctionTable_Pre, 0, sizeof(NEW_DLL_FUNCTIONS));
    memcpy(pNewFunctionTable, &g_DLL_NewFunctionTable_Pre, sizeof(NEW_DLL_FUNCTIONS));
    return 1;
}

C_DLLEXPORT int GetNewDLLFunctions_Post(NEW_DLL_FUNCTIONS *pNewFunctionTable, int *interfaceVersion) {
    memset(&g_DLL_NewFunctionTable_Post, 0, sizeof(NEW_DLL_FUNCTIONS));
    memcpy(pNewFunctionTable, &g_DLL_NewFunctionTable_Post, sizeof(NEW_DLL_FUNCTIONS));
    return 1;
}

// ✅ GiveFnptrsToDll
C_DLLEXPORT void WINAPI GiveFnptrsToDll(enginefuncs_t *pengfuncsFromEngine, globalvars_t *pGlobals) {
    memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
    gpGlobals = pGlobals;
}

// ✅ Meta_Attach
C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME now, META_FUNCTIONS *pFunctionTable,
                            meta_globals_t *pMGlobals, gamedll_funcs_t *pGamedllFuncs) {
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

    if (g_pLogApi == nullptr) g_pLogApi = new CLogApi();
    if (g_pLogCurl == nullptr) g_pLogCurl = new CLogCurl();

    ReAPI_Init();
    ReGameDLL_Init();

    return TRUE;
}

// ✅ Meta_Detach
C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME now, PL_UNLOAD_REASON reason) {
    ReAPI_Stop();
    ReGameDLL_Stop();

    if (g_pLogApi != nullptr) {
        delete g_pLogApi;
        g_pLogApi = nullptr;
    }
    if (g_pLogCurl != nullptr) {
        delete g_pLogCurl;
        g_pLogCurl = nullptr;
    }

    return TRUE;
}

// ✅ Meta_Query
C_DLLEXPORT int Meta_Query(char *interfaceVersion, plugin_info_t **pPlugInfo,
                           mutil_funcs_t *pMetaUtilFuncs) {
    *pPlugInfo = &Plugin_info;
    gpMetaUtilFuncs = pMetaUtilFuncs;
    return TRUE;
}

#pragma endregion

// ✅ DLL_POST Functions (Must be defined somewhere in the project)
void DLL_POST_ServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
void DLL_POST_ServerDeactivate(void);
void DLL_POST_StartFrame(void);
qboolean DLL_POST_ClientConnect(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);
void DLL_POST_ClientPutInServer(edict_t *pEntity);
void DLL_POST_ClientKill(edict_t *pEntity);
void DLL_POST_ClientUserInfoChanged(edict_t *pEntity, char *InfoBuffer);
void DLL_POST_ClientCommand(edict_t *pEntity);

// ✅ ENGINE_POST Functions
void ENGINE_POST_AlertMessage(ALERT_TYPE atype, const char *szFmt, ...);
