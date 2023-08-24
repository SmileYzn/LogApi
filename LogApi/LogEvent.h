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
	};
}

class CLogEvent
{
public:
	void ServerActivate(edict_t* pEdictList, int edictCount, int clientMax);
	void ServerDeactivate();
	void ServerAlertMessage(ALERT_TYPE aType, const char* szBuffer);
	void ServerInfo();
	void ClientConnect(edict_t* pEntity, const char* pszName, const char* pszAddress, char szRejectReason[128]);
	void ClientPutInServer(edict_t* pEntity);
	void ClientDisconnect(edict_t* pEntity, bool Crash, const char* Reason);
	void ClientKill(edict_t* pEntity);
	void ClientUserInfoChanged(edict_t* pEntity, char* InfoBuffer);
	void ClientCommand(edict_t* pEntity);
	void ClientSay(edict_t* pEntity);
private:
	nlohmann::ordered_json m_Event;
};

extern CLogEvent gLogEvent;