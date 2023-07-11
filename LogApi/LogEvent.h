#pragma once

namespace LogApi
{
	enum Events
	{
		ServerActivate,
		ServerDeactivate,
		ServerAlertMessage,
		ClientConnect,
		ClientPutInServer,
		ClientDisconnect,
		ClientKill,
		ClientUserInfoChanged,
	};
}

class CLogEvent
{
public:
	void ServerActivate(edict_t* pEdictList, int edictCount, int clientMax);
	void ServerDeactivate();
	void ServerAlertMessage(ALERT_TYPE aType, const char* szBuffer);
	void ClientConnect(edict_t* pEntity, const char* pszName, const char* pszAddress, char szRejectReason[128]);
	void ClientPutInServer(edict_t* pEntity);
	void ClientDisconnect(edict_t* pEntity);
	void ClientKill(edict_t* pEntity);
	void ClientUserInfoChanged(edict_t* pEntity, char* InfoBuffer);
private:
	// Current event data
	nlohmann::ordered_json m_Event;
};

extern CLogEvent gLogEvent;