#pragma once

namespace LogApi
{
	enum Events
	{
		ServerActivate,
		ServerDeactivate,
		ServerAlertMessage,
		ServerInfo,
		ClientConnect,
		ClientPutInServer,
		ClientDisconnect,
		ClientKill,
		ClientUserInfoChanged,
		ClientCommand,
		ClientSay,
		ClientMenuHandle
	};
}

class CLogEvent
{
public:
	void ServerActivate(edict_t* pEdictList, int edictCount, int clientMax);
	void ServerDeactivate();
	void ServerAlertMessage(ALERT_TYPE aType, const char* szBuffer);
	void ServerInfo();
	void ClientConnect(edict_t* pEdict, const char* pszName, const char* pszAddress, char szRejectReason[128]);
	void ClientPutInServer(edict_t* pEdict);
	void ClientDisconnect(edict_t* pEdict, bool Crash, const char* Reason);
	void ClientKill(edict_t* pEdict);
	void ClientUserInfoChanged(edict_t* pEdict, char* InfoBuffer);
	void ClientCommand(edict_t* pEdict);
	void ClientSay(edict_t* pEdict);
	void ClientMenuHandle(edict_t* pEdict, std::string Callback, P_MENU_ITEM Item);
private:
	nlohmann::ordered_json m_Event;
};

extern CLogEvent gLogEvent;