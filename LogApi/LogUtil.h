#pragma once

class CLogUtil
{
public:
	cvar_t* CvarRegister(const char* Name, const char* Value);
	void ServerExecute(std::string Command);

private:
	std::map<std::string, cvar_t> m_Cvar;
};

extern CLogUtil gLogUtil;