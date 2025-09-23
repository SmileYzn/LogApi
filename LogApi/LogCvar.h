#pragma once

class CLogCvar
{
public:
    // On Server Activate
    void ServerActivate();

    // Register Cvar
    cvar_t* Register(const char *pszName, const char *pszValue);

	// Console variables
	cvar_t* m_Enable;
	cvar_t* m_Address;
	cvar_t* m_Timeout;
	cvar_t* m_Bearer;
	cvar_t* m_Delay;

    // Public cvar data
    std::map<std::string, cvar_t> m_Data;
};

extern CLogCvar gLogCvar;