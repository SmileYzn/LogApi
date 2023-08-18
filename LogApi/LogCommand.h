#pragma once

class CLogCommand
{
public:
	void ServerActivate();

	static void Say();
	static void TeamSay();
	static void CenterSay();
	static void PrivateSay();

	hudtextparms_t GetHudParameters(bool TeamSay)
	{
		hudtextparms_t hud = { -1.0f, -1.0f, 1, 255, 255, 255, 255, 0, 255, 0, 255, 0.5f, 0.5f, 6.0f, 6.0f, -1 };

		if (++this->m_MessageLine > 6 || this->m_MessageLine < 3)
		{
			this->m_MessageLine = 3;
		}

		hud.x = TeamSay ? 0.05 : -1.0;

		hud.y = ((TeamSay ? 0.55f : 0.1f) + float(this->m_MessageLine) / 30.0f);

		hud.channel = (this->m_MessageLine - 2);

		return hud;
	}

private:
	int m_MessageLine = 0;
};

extern CLogCommand gLogCommand;

