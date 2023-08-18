#include "precompiled.h"

CLogCommand gLogCommand;

void CLogCommand::ServerActivate()
{
	g_engfuncs.pfnAddServerCommand(LOG_COMMAND_LIST[0], this->Say);
	g_engfuncs.pfnAddServerCommand(LOG_COMMAND_LIST[1], this->TeamSay);
	g_engfuncs.pfnAddServerCommand(LOG_COMMAND_LIST[2], this->CenterSay);
	g_engfuncs.pfnAddServerCommand(LOG_COMMAND_LIST[3], this->PrivateSay);
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
				gLogUtil.ClientPrint(nullptr, PRINT_CHAT, "%s", Message);
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

	LOG_CONSOLE(PLID, "[%s] Usage: log_say <message>", Plugin_info.logtag);
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

	LOG_CONSOLE(PLID, "[%s] Usage: log_cay <message>", Plugin_info.logtag);
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
							Message.erase(0, Target.size());

							gLogUtil.ClientPrint(Player->edict(), PRINT_CHAT, "%s", Message.c_str());
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
