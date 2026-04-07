#include "precompiled.h"

CLogCvar gLogCvar;

// On Server Activate
void CLogCvar::ServerActivate()
{
	// // Enable Log API (0 Disable, 1 Enable)
	this->m_Enable = this->Register("log_api_enable", "0");

	// // Set Log API Address (API HTTP/s Address Ie. https://api.yoursite.com/)
	this->m_Address = this->Register("log_api_address", "");

	// // Set Log API Timeout (Timeout in seconds to wait for response from remote server)
	this->m_Timeout = this->Register("log_api_timeout", "5.0");

	// // Set Log API Bearer Token (Authentication Token or leave empty to disable)
	this->m_Bearer = this->Register("log_api_bearer", "");

	// // Server Info Event Delay (Delay to update Server Info on webserver)
	this->m_Delay = this->Register("log_api_delay", "60.0");

	// // Enable Remote Command Execution (0 Disable, 1 Enable)
	this->m_ExecCommands = this->Register("log_api_exec_commands", "0");

	// // Execute Settings File
	g_engfuncs.pfnServerCommand("exec addons/logapi/logapi.cfg\n");
}

// Register Cvar
cvar_t *CLogCvar::Register(const char *pszName, const char *pszValue)
{
    // Try to get pointer
    cvar_t *pPointer = g_engfuncs.pfnCVarGetPointer(pszName);

    // If is null
    if (!pPointer)
    {
        // If name is not null
        if (pszName)
        {
            // If string is not empty
            if (pszName[0u] != '\0')
            {
                // Store name
                this->m_Data[pszName].name = pszName;

                // Store value
                this->m_Data[pszName].string = "";

                // Store flags
                this->m_Data[pszName].flags = (FCVAR_SERVER | FCVAR_PROTECTED | FCVAR_SPONLY | FCVAR_UNLOGGED);

                // Register cvar data
                g_engfuncs.pfnCVarRegister(&this->m_Data[pszName]);

                // Get new pointer
                pPointer = g_engfuncs.pfnCVarGetPointer(this->m_Data[pszName].name);

                // If is not null
                if (pPointer)
                {
                    // Set the value
                    g_engfuncs.pfnCvar_DirectSet(pPointer, pszValue);
                }
            }
        }
    }

    // Return already registered pointer
    return pPointer;
}