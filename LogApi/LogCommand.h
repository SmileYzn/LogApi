#pragma once

class CLogCommand
{
public:
	// On Server Activate
	void ServerActivate();

	// Say
	static void Say();

	// Team SAy
	static void TeamSay();

	// Center Say
	static void CenterSay();

	// Private Say
	static void PrivateSay();

	// Console Log
	static void ConsoleLog();

	// Open MOTD
	static void OpenMotd();

	// Send Server Information
	static void ServerInfo();

	// Get Hudmessage Parameters
	hudtextparms_t GetHudParameters(bool TeamSay);

private:
	int m_HudLine = 0;
};

extern CLogCommand gLogCommand;

