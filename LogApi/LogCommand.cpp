#include "precompiled.h"

CLogCommand gLogCommand;

void CLogCommand::ServerActivate()
{
	g_engfuncs.pfnAddServerCommand("log_say", this->Say);
}

void CLogCommand::Say()
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

	LOG_CONSOLE(PLID, "[%s] Usage: log_say <message>", Plugin_info.logtag);
}
