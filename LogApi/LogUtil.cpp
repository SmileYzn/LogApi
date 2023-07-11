#include "precompiled.h"

CLogUtil gLogUtil;

cvar_t* CLogUtil::CvarRegister(const char* Name, const char* Value)
{
	cvar_t* Pointer = g_engfuncs.pfnCVarGetPointer(Name);

	if (!Pointer)
	{
		this->m_Cvar[Name].name = Name;

		this->m_Cvar[Name].string = (char*)(Value);

		this->m_Cvar[Name].flags = (FCVAR_SERVER | FCVAR_PROTECTED | FCVAR_SPONLY | FCVAR_UNLOGGED);

		g_engfuncs.pfnCVarRegister(&this->m_Cvar[Name]);

		Pointer = g_engfuncs.pfnCVarGetPointer(this->m_Cvar[Name].name);

		if (Pointer)
		{
			g_engfuncs.pfnCvar_DirectSet(Pointer, Value);
		}
	}

	return Pointer;
}

void CLogUtil::ServerExecute(std::string CommandData)
{
	if (!CommandData.empty())
	{
		std::ofstream File(LOG_API_TEMP_FILE, std::ofstream::binary);

		if (File)
		{
			File.write(CommandData.c_str(), CommandData.size());
		}

		File.close();
	}

	char ExecuteCommand[MAX_PATH] = { 0 };

	Q_snprintf(ExecuteCommand, sizeof(ExecuteCommand), "exec %s\n", LOG_API_TEMP_FILE);

	if(ExecuteCommand[0])
	{
		g_engfuncs.pfnServerCommand(ExecuteCommand);
	}
}
