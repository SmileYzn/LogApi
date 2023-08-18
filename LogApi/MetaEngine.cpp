#include "precompiled.h"

// ENGINE POST Functions Table
enginefuncs_t gENGINE_FunctionTable_Pre;

// ENGINE POST Functions Table
enginefuncs_t gENGINE_FunctionTable_Post;

#pragma region ENGINE_PRE
C_DLLEXPORT int GetEngineFunctions(enginefuncs_t* pengfuncsFromEngine, int* interfaceVersion)
{
	memset(&gENGINE_FunctionTable_Pre, 0, sizeof(enginefuncs_t));

	// Register Functions Here //

	memcpy(pengfuncsFromEngine, &gENGINE_FunctionTable_Pre, sizeof(enginefuncs_t));

	return 1;
}
#pragma endregion

#pragma region ENGINE_POST
C_DLLEXPORT int GetEngineFunctions_Post(enginefuncs_t* pengfuncsFromEngine, int* interfaceVersion)
{
	memset(&gENGINE_FunctionTable_Post, 0, sizeof(enginefuncs_t));

	gENGINE_FunctionTable_Post.pfnAlertMessage = ENGINE_POST_AlertMessage;

	memcpy(pengfuncsFromEngine, &gENGINE_FunctionTable_Post, sizeof(enginefuncs_t));

	return 1;
}

void ENGINE_POST_AlertMessage(ALERT_TYPE atype, const char* szFmt, ...)
{
	if (atype == at_logged)
	{
		static char szBuffer[2048];

		va_list vArgList;
		va_start(vArgList, szFmt);
		vsprintf(szBuffer, szFmt, vArgList);
		va_end(vArgList);

		if (szBuffer[0])
		{
			gLogEvent.ServerAlertMessage(atype, szBuffer);
		}
	}

	RETURN_META(MRES_IGNORED);
}
#pragma endregion

