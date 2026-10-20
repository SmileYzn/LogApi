#include "precompiled.h"

CLogEvent gLogEvent;

void CLogEvent::ServerActivate(edict_t* pEdictList, int edictCount, int clientMax)
{
	if (gLogApi.EventEnabled(__func__))
	{ 
		nlohmann::ordered_json Event;

		Event["Event"] = __func__;

		Event["Server"] = gLogApi.GetServerInfo();

		Event["EdictCount"] = edictCount;

		Event["ClientMax"] = clientMax;

		gLogApi.SendEvent(LogApi::Events::ServerActivate, Event);
	}
}

void CLogEvent::ServerDeactivate()
{
	if (gLogApi.EventEnabled(__func__))
	{
		nlohmann::ordered_json Event;

		Event["Event"] = __func__;

		Event["Server"] = gLogApi.GetServerInfo();

		gLogApi.SendEvent(LogApi::Events::ServerDeactivate, Event);
	}
}

void CLogEvent::ServerAlertMessage(ALERT_TYPE aType, const char* szBuffer)
{
	if (gLogApi.EventEnabled(__func__))
	{
		if (szBuffer)
		{
			if (szBuffer[0u] != '\0')
			{
				nlohmann::ordered_json Event;

				Event["Event"] = __func__;

				Event["Server"] = gLogApi.GetServerInfo();

				Event["Type"] = aType;

				Event["Message"] = szBuffer;

				gLogApi.SendEvent(LogApi::Events::ServerAlertMessage, Event);
			}
		}
	}
}

void CLogEvent::ServerInfo()
{
	if (gLogApi.EventEnabled(__func__))
	{
		nlohmann::ordered_json Event;

		Event["Event"] = __func__;

		Event["Server"] = gLogApi.GetServerInfo();

		gLogApi.SendEvent(LogApi::Events::ServerInfo, Event);
	}
}

void CLogEvent::ClientConnect(edict_t* pEdict, const char* pszName, const char* pszAddress, char szRejectReason[128])
{
	if (!gLogApi.EventEnabled(__func__))
		return;

	// Guard: entity must be fully valid with private data allocated
	if (FNullEnt(pEdict) || !pEdict->pvPrivateData)
		return;

	nlohmann::ordered_json Event;

	Event["Event"] = __func__;

	Event["Server"] = gLogApi.GetServerInfo();

	Event["Player"] = gLogPlayer.GetPlayerJson(pEdict);

	if (Event.empty())
		return;

	gLogApi.SendEvent(LogApi::Events::ClientConnect, Event);
}

void CLogEvent::ClientPutInServer(edict_t* pEdict)
{
	if (!gLogApi.EventEnabled(__func__))
		return;

	if (FNullEnt(pEdict) || !pEdict->pvPrivateData)
		return;

	nlohmann::ordered_json Event;

	Event["Event"] = __func__;

	Event["Server"] = gLogApi.GetServerInfo();

	Event["Player"] = gLogPlayer.GetPlayerJson(pEdict);

	if (Event.empty())
		return;

	gLogApi.SendEvent(LogApi::Events::ClientPutInServer, Event);
}

void CLogEvent::ClientDisconnect(edict_t* pEdict, bool Crash, const char* Reason)
{
	if (!gLogApi.EventEnabled(__func__))
		return;

	// During disconnect pvPrivateData may already be freed — guard both checks
	if (FNullEnt(pEdict) || !pEdict->pvPrivateData)
		return;

	nlohmann::ordered_json Event;

	Event["Event"] = __func__;

	Event["Server"] = gLogApi.GetServerInfo();

	// GetPlayerJson reads from our internal player cache, not directly from pdata
	Event["Player"] = gLogPlayer.GetPlayerJson(pEdict);

	Event["Crash"] = Crash;

	// Copy reason immediately — engine buffer may be freed after this call returns
	std::string SafeReason = (Reason && Reason[0u] != '\0') ? Reason : "";
	Event["Reason"] = SafeReason;

	if (Event.empty())
		return;

	gLogApi.SendEvent(LogApi::Events::ClientDisconnect, Event);
}

void CLogEvent::ClientKill(edict_t* pEdict)
{
	if (!gLogApi.EventEnabled(__func__))
		return;

	if (FNullEnt(pEdict) || !pEdict->pvPrivateData)
		return;

	nlohmann::ordered_json Event;

	Event["Event"] = __func__;

	Event["Server"] = gLogApi.GetServerInfo();

	Event["Player"] = gLogPlayer.GetPlayerJson(pEdict);

	if (Event.empty())
		return;

	gLogApi.SendEvent(LogApi::Events::ClientKill, Event);
}

void CLogEvent::ClientUserInfoChanged(edict_t* pEdict, char* InfoBuffer)
{
	if (!gLogApi.EventEnabled(__func__))
		return;

	if (FNullEnt(pEdict) || !pEdict->pvPrivateData)
		return;

	// Copy immediately — InfoBuffer is a transient engine pointer that may be
	// invalidated by subsequent engine calls or after this handler returns
	std::string SafeInfo = (InfoBuffer && InfoBuffer[0u] != '\0') ? InfoBuffer : "";

	nlohmann::ordered_json Event;

	Event["Event"] = __func__;

	Event["Server"] = gLogApi.GetServerInfo();

	Event["Player"] = gLogPlayer.GetPlayerJson(pEdict);

	Event["InfoBuffer"] = SafeInfo;

	if (Event.empty())
		return;

	gLogApi.SendEvent(LogApi::Events::ClientUserInfoChanged, Event);
}

void CLogEvent::ClientCommand(edict_t* pEdict)
{
	if (!gLogApi.EventEnabled(__func__))
		return;

	if (FNullEnt(pEdict) || !pEdict->pvPrivateData)
		return;

	// Capture engine argv/args pointers ONCE and deep-copy them immediately.
	// These are transient internal engine buffers invalidated by re-entrant calls.
	const char* rawCmd = g_engfuncs.pfnCmd_Argv(0);
	if (!rawCmd || rawCmd[0u] == '\0')
		return;
	std::string Command(rawCmd);

	const char* rawArgs = g_engfuncs.pfnCmd_Args();
	std::string Args = (rawArgs && rawArgs[0u] != '\0') ? rawArgs : "";

	nlohmann::ordered_json Event;

	Event["Event"] = __func__;

	Event["Server"] = gLogApi.GetServerInfo();

	Event["Player"] = gLogPlayer.GetPlayerJson(pEdict);

	Event["Command"] = Command;

	Event["Args"] = Args;

	if (Event.empty())
		return;

	gLogApi.SendEvent(LogApi::Events::ClientCommand, Event);
}

void CLogEvent::ClientSay(edict_t* pEdict)
{
	if (!gLogApi.EventEnabled(__func__))
		return;

	if (FNullEnt(pEdict) || !pEdict->pvPrivateData)
		return;

	// Capture argv(0) once and copy immediately — transient engine pointer
	const char* rawType = g_engfuncs.pfnCmd_Argv(0);
	if (!rawType || rawType[0u] == '\0')
		return;
	std::string Type(rawType);

	if (Type != "say" && Type != "say_team")
		return;

	// Capture args once and copy immediately
	const char* rawMsg = g_engfuncs.pfnCmd_Args();
	if (!rawMsg || rawMsg[0u] == '\0')
		return;
	std::string Message(rawMsg);

	// Strip enclosing quotes the engine wraps around chat messages
	Message.erase(std::remove(Message.begin(), Message.end(), '\"'), Message.end());

	if (Message.empty())
		return;

	nlohmann::ordered_json Event;

	Event["Event"] = __func__;

	Event["Server"] = gLogApi.GetServerInfo();

	Event["Player"] = gLogPlayer.GetPlayerJson(pEdict);

	Event["Type"] = Type;

	Event["Message"] = Message;

	if (Event.empty())
		return;

	gLogApi.SendEvent(LogApi::Events::ClientSay, Event);
}

void CLogEvent::ClientMenuHandle(edict_t* pEdict, std::string Callback, P_MENU_ITEM Item)
{
	if (!gLogApi.EventEnabled(__func__))
		return;

	if (Callback.empty())
		return;

	if (FNullEnt(pEdict) || !pEdict->pvPrivateData)
		return;

	nlohmann::ordered_json Event;

	Event["Event"] = Callback;

	Event["Server"] = gLogApi.GetServerInfo();

	Event["Player"] = gLogPlayer.GetPlayerJson(pEdict);

	// P_MENU_ITEM is passed by value — the std::string members are already safe
	// deep copies of whatever the caller had. Assign with empty-string fallback.
	Event["Item"]["Info"]     = Item.Info.empty()  ? "" : Item.Info;
	Event["Item"]["Text"]     = Item.Text.empty()  ? "" : Item.Text;
	Event["Item"]["Disabled"] = Item.Disabled;
	Event["Item"]["Extra"]    = Item.Extra.empty() ? "" : Item.Extra;

	if (Event.empty())
		return;

	gLogApi.SendEvent(LogApi::Events::ClientMenuHandle, Event);
}
