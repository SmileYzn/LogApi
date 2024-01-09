#include "precompiled.h"

CLogCommand gLogCommand;

void CLogCommand::ServerActivate()
{
	g_engfuncs.pfnAddServerCommand("log_say", this->Say);
	g_engfuncs.pfnAddServerCommand("log_tsay", this->TeamSay);
	g_engfuncs.pfnAddServerCommand("log_csay", this->CenterSay);
	g_engfuncs.pfnAddServerCommand("log_psay", this->PrivateSay);
	g_engfuncs.pfnAddServerCommand("log_console", this->ConsoleLog);
	g_engfuncs.pfnAddServerCommand("log_motd", this->OpenMotd);
	g_engfuncs.pfnAddServerCommand("log_send_info", this->ServerInfo);
}

void CLogCommand::Say()
{
	if (g_engfuncs.pfnCmd_Argc() >= 2)
	{
		auto Message = g_engfuncs.pfnCmd_Args();

		if (Message)
		{
			if (Message[0u] != '\0')
			{
				gLogUtil.SayText(nullptr, PRINT_TEAM_DEFAULT, "%s", Message);
				return;
			}
		}
	}

	LOG_CONSOLE(PLID, "[%s] Usage: log_say <message>", Plugin_info.logtag);
}

void CLogCommand::TeamSay()
{
	if (g_engfuncs.pfnCmd_Argc() >= 2)
	{
		auto Message = g_engfuncs.pfnCmd_Args();

		if (Message)
		{
			if (Message[0u] != '\0')
			{
				gLogUtil.HudMessage(nullptr, gLogCommand.GetHudParameters(true), "%s", Message);
				return;
			}
		}
	}

	LOG_CONSOLE(PLID, "[%s] Usage: log_tsay <message>", Plugin_info.logtag);
}

void CLogCommand::CenterSay()
{
	if (g_engfuncs.pfnCmd_Argc() >= 2)
	{
		auto Message = g_engfuncs.pfnCmd_Args();

		if (Message)
		{
			if (Message[0u] != '\0')
			{
				gLogUtil.HudMessage(nullptr, gLogCommand.GetHudParameters(false), "%s", Message);
				return;
			}
		}
	}

	LOG_CONSOLE(PLID, "[%s] Usage: log_csay <message>", Plugin_info.logtag);
}

void CLogCommand::PrivateSay()
{
	if (g_engfuncs.pfnCmd_Argc() >= 3)
	{
		std::string Target = g_engfuncs.pfnCmd_Argv(1);

		if (!Target.empty())
		{
			if (Target.length() > 1)
			{
				std::string Message = g_engfuncs.pfnCmd_Args();

				if (!Message.empty())
				{
					if (Message.length() > 0)
					{
						auto Player = gLogUtil.FindPlayer(Target);

						if (Player)
						{
							Message.erase(0, Target.length() + 1);

							gLogUtil.SayText(Player->edict(), Player->entindex(), "%s", Message.c_str());
						}
						else
						{
							LOG_CONSOLE(PLID, "[%s] Client with that name or userid '%s' not found.", Plugin_info.logtag, Target.c_str());
						}

						return;
					}
				}
			}
		}
	}

	LOG_CONSOLE(PLID, "[%s] Usage: log_psay <name or #userid> <message>", Plugin_info.logtag);
}

void CLogCommand::ConsoleLog()
{
	if (g_engfuncs.pfnCmd_Argc() >= 3)
	{
		std::string Target = g_engfuncs.pfnCmd_Argv(1);

		if (!Target.empty())
		{
			if (Target.length() > 1)
			{
				std::string Message = g_engfuncs.pfnCmd_Args();

				if (!Message.empty())
				{
					if (Message.length() > 0)
					{
						auto Player = gLogUtil.FindPlayer(Target);

						if (Player)
						{
							Message.erase(0, Target.length() + 1);

							gLogUtil.ClientPrint(Player->edict(), PRINT_CONSOLE, "%s", Message.c_str());
						}
						else
						{
							LOG_CONSOLE(PLID, "[%s] Client with that name or userid '%s' not found.", Plugin_info.logtag, Target.c_str());
						}

						return;
					}
				}
			}
		}
	}

	LOG_CONSOLE(PLID, "[%s] Usage: log_console <name or #userid> <message>", Plugin_info.logtag);
}

void CLogCommand::OpenMotd()
{
	if (g_engfuncs.pfnCmd_Argc() >= 3)
	{
		std::string Target = g_engfuncs.pfnCmd_Argv(1);

		if (!Target.empty())
		{
			if (Target.length() > 1)
			{
				std::string Message = g_engfuncs.pfnCmd_Args();

				if (!Message.empty())
				{
					if (Message.length() > 0)
					{
						auto Player = gLogUtil.FindPlayer(Target);

						if (Player)
						{
							Message.erase(0, Target.length() + 1);

							char Path[MAX_MOTD_LENGTH] = { 0 };

							Q_memset(Path, 0, sizeof(Path));

							Q_strncpy(Path, Message.data(), sizeof(Path));

							gLogUtil.ShowMotd(Player->edict(), Path, strlen(Path));

							return;
						}
						else
						{
							LOG_CONSOLE(PLID, "[%s] Client with that name or userid '%s' not found.", Plugin_info.logtag, Target.c_str());
						}

						return;
					}
				}
			}
		}
	}

	LOG_CONSOLE(PLID, "[%s] Usage: log_motd <name or #userid> <webpage or file name>", Plugin_info.logtag);
}

void CLogCommand::ServerInfo()
{
	gLogEvent.ServerInfo();

	LOG_CONSOLE(PLID, "[%s] Server info sent to webserver.", Plugin_info.logtag);
}