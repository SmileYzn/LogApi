#include "precompiled.h"

CLogEvent gLogEvent;

void CLogEvent::ServerActivate(edict_t* pEdictList, int edictCount, int clientMax)
{
	if (gLogApi.EventEnabled(__func__))
	{ 
		this->m_Event.clear();

		this->m_Event["Event"] = __func__;

		this->m_Event["Server"] = gLogApi.GetServerInfo();

		this->m_Event["EdictCount"] = edictCount;

		this->m_Event["ClientMax"] = clientMax;

		gLogApi.SendEvent(LogApi::Events::ServerActivate, this->m_Event);
	}
}

void CLogEvent::ServerDeactivate()
{
	if (gLogApi.EventEnabled(__func__))
	{
		this->m_Event.clear();

		this->m_Event["Event"] = __func__;

		this->m_Event["Server"] = gLogApi.GetServerInfo();

		gLogApi.SendEvent(LogApi::Events::ServerDeactivate, this->m_Event);
	}
}

void CLogEvent::ServerAlertMessage(ALERT_TYPE aType, const char* szBuffer)
{
	if (gLogApi.EventEnabled(__func__))
	{
		if (szBuffer)
		{
			this->m_Event.clear();

			this->m_Event["Event"] = __func__;

			this->m_Event["Server"] = gLogApi.GetServerInfo();

			this->m_Event["Type"] = aType;

			this->m_Event["Message"] = szBuffer;

			gLogApi.SendEvent(LogApi::Events::ServerAlertMessage, this->m_Event);
		}
	}
}

void CLogEvent::ServerInfo()
{
	if (gLogApi.EventEnabled(__func__))
	{
		this->m_Event.clear();

		this->m_Event["Event"] = __func__;

		this->m_Event["Server"] = gLogApi.GetServerInfo();

		gLogApi.SendEvent(LogApi::Events::ServerInfo, this->m_Event);
	}
}

void CLogEvent::ClientConnect(edict_t* pEntity, const char* pszName, const char* pszAddress, char szRejectReason[128])
{
	if (gLogApi.EventEnabled(__func__))
	{
		this->m_Event.clear();

		if (!FNullEnt(pEntity))
		{
			this->m_Event["Event"] = __func__;

			this->m_Event["Server"] = gLogApi.GetServerInfo();

			this->m_Event["UserId"] = g_engfuncs.pfnGetPlayerUserId(pEntity);

			this->m_Event["Name"] = pszName;

			this->m_Event["AuthId"] = g_engfuncs.pfnGetPlayerAuthId(pEntity);

			this->m_Event["Address"] = pszAddress;
		}

		gLogApi.SendEvent(LogApi::Events::ClientConnect, this->m_Event);
	}
}

void CLogEvent::ClientPutInServer(edict_t* pEntity)
{
	if (gLogApi.EventEnabled(__func__))
	{
		this->m_Event.clear();

		if (!FNullEnt(pEntity))
		{
			this->m_Event["Event"] = __func__;

			this->m_Event["Server"] = gLogApi.GetServerInfo();

			this->m_Event["UserId"] = g_engfuncs.pfnGetPlayerUserId(pEntity);

			this->m_Event["Name"] = STRING(pEntity->v.netname);

			this->m_Event["AuthId"] = g_engfuncs.pfnGetPlayerAuthId(pEntity);
		}

		gLogApi.SendEvent(LogApi::Events::ClientPutInServer, this->m_Event);
	}
}

void CLogEvent::ClientDisconnect(edict_t* pEntity)
{
	if (gLogApi.EventEnabled(__func__))
	{
		this->m_Event.clear();

		if (!FNullEnt(pEntity))
		{
			this->m_Event["Event"] = __func__;

			this->m_Event["Server"] = gLogApi.GetServerInfo();

			this->m_Event["UserId"] = g_engfuncs.pfnGetPlayerUserId(pEntity);

			this->m_Event["Name"] = STRING(pEntity->v.netname);

			this->m_Event["AuthId"] = g_engfuncs.pfnGetPlayerAuthId(pEntity);
		}

		gLogApi.SendEvent(LogApi::Events::ClientDisconnect, this->m_Event);
	}
}

void CLogEvent::ClientKill(edict_t* pEntity)
{
	if (gLogApi.EventEnabled(__func__))
	{
		this->m_Event.clear();

		if (!FNullEnt(pEntity))
		{
			this->m_Event["Event"] = __func__;

			this->m_Event["Server"] = gLogApi.GetServerInfo();
			
			this->m_Event["UserId"] = g_engfuncs.pfnGetPlayerUserId(pEntity);

			this->m_Event["Name"] = STRING(pEntity->v.netname);

			this->m_Event["AuthId"] = g_engfuncs.pfnGetPlayerAuthId(pEntity);
		}

		gLogApi.SendEvent(LogApi::Events::ClientKill, this->m_Event);
	}
}

void CLogEvent::ClientUserInfoChanged(edict_t* pEntity, char* InfoBuffer)
{
	if (gLogApi.EventEnabled(__func__))
	{
		this->m_Event.clear();

		if (!FNullEnt(pEntity))
		{
			this->m_Event["Event"] = __func__;

			this->m_Event["Server"] = gLogApi.GetServerInfo();

			this->m_Event["UserId"] = g_engfuncs.pfnGetPlayerUserId(pEntity);

			this->m_Event["Name"] = STRING(pEntity->v.netname);

			this->m_Event["AuthId"] = g_engfuncs.pfnGetPlayerAuthId(pEntity);

			this->m_Event["InfoBuffer"] = InfoBuffer;
		}

		gLogApi.SendEvent(LogApi::Events::ClientUserInfoChanged, this->m_Event);
	}
}

void CLogEvent::ClientCommand(edict_t* pEntity)
{
	if (gLogApi.EventEnabled(__func__))
	{
		this->m_Event.clear();

		if (!FNullEnt(pEntity))
		{
			auto Command = g_engfuncs.pfnCmd_Argv(0);

			if (Command)
			{
				if (Command[0u] != '\0')
				{
					this->m_Event["Event"] = __func__;

					this->m_Event["Server"] = gLogApi.GetServerInfo();

					this->m_Event["UserId"] = g_engfuncs.pfnGetPlayerUserId(pEntity);

					this->m_Event["Name"] = STRING(pEntity->v.netname);

					this->m_Event["AuthId"] = g_engfuncs.pfnGetPlayerAuthId(pEntity);

					this->m_Event["Command"] = Command;

					this->m_Event["Args"] = "";

					auto Args = g_engfuncs.pfnCmd_Args();

					if (Args)
					{
						if (Args[0u] != '\0')
						{
							this->m_Event["Args"] = Args;
						}
					}
				}
			}
		}

		gLogApi.SendEvent(LogApi::Events::ClientCommand, this->m_Event);
	}
}

void CLogEvent::ClientSay(edict_t* pEntity)
{
	if (gLogApi.EventEnabled(__func__))
	{
		this->m_Event.clear();

		if (!FNullEnt(pEntity))
		{
			std::string Type = g_engfuncs.pfnCmd_Argv(0) ? g_engfuncs.pfnCmd_Argv(0) : "";

			if (!Type.empty())
			{
				if (Type.length() > 0)
				{
					if (Type.compare("say") == 0 || Type.compare("say_team") == 0)
					{
						std::string Message = g_engfuncs.pfnCmd_Args() ? g_engfuncs.pfnCmd_Args() : "";

						if (!Message.empty())
						{
							if (Message.length() > 0)
							{
								Message.erase(std::remove(Message.begin(), Message.end(), '\"'),Message.end());

								this->m_Event["Event"] = __func__;

								this->m_Event["Server"] = gLogApi.GetServerInfo();

								this->m_Event["UserId"] = g_engfuncs.pfnGetPlayerUserId(pEntity);

								this->m_Event["Name"] = STRING(pEntity->v.netname);

								this->m_Event["AuthId"] = g_engfuncs.pfnGetPlayerAuthId(pEntity);

								this->m_Event["Type"] = Type;

								this->m_Event["Message"] = Message;
							}
						}
					}
				}
			}
		}

		gLogApi.SendEvent(LogApi::Events::ClientSay, this->m_Event);
	}
}
