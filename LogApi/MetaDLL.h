#pragma once

#pragma region DLL_PRE
#pragma endregion

#pragma region DLL_POST
void DLL_POST_ServerActivate(edict_t* pEdictList, int edictCount, int clientMax);
void DLL_POST_ServerDeactivate(void);
void DLL_POST_StartFrame();
qboolean DLL_POST_ClientConnect(edict_t* pEntity, const char* pszName, const char* pszAddress, char szRejectReason[128]);
void DLL_POST_ClientPutInServer(edict_t* pEntity);
void DLL_POST_ClientKill(edict_t* pEntity);
void DLL_POST_ClientUserInfoChanged(edict_t* pEntity, char* InfoBuffer);
void DLL_POST_ClientCommand(edict_t* pEntity);
#pragma endregion