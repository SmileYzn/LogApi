#include "precompiled.h"

std::array<CLogMenu, MAX_CLIENTS + 1> gLogMenu;

void CLogMenu::Clear()
{
	this->m_Text = "";

	this->m_Data.clear();

	this->m_Page = -1;

	this->m_Exit = false;

	this->m_Func = nullptr;
}

void CLogMenu::Create(std::string Title, std::string Callback, bool Exit, void* CallbackFunction)
{
	this->Clear();

	this->m_Text = Title;

	this->m_Callback = Callback;

	this->m_Page = -1;

	this->m_Exit = Exit;

	this->m_Func = CallbackFunction;
}

void CLogMenu::AddItem(std::string Info, std::string Text, bool Disabled, std::string Extra)
{
	P_MENU_ITEM Item = { Info, Text, Disabled, Extra };

	this->m_Data.push_back(Item);
}

void CLogMenu::Show(int EntityIndex)
{
	if (this->m_Data.size())
	{
		this->Display(EntityIndex, 0);
	}
}

void CLogMenu::Hide(int EntityIndex)
{
	this->m_Page = -1;

	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		Player->m_iMenu = Menu_OFF;

		if (!Player->IsDormant() && !Player->IsBot())
		{
			static int iMsgShowMenu;

			if (iMsgShowMenu || (iMsgShowMenu = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "ShowMenu", NULL)))
			{
				g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgShowMenu, nullptr, Player->edict());
				g_engfuncs.pfnWriteShort(0);
				g_engfuncs.pfnWriteChar(0);
				g_engfuncs.pfnWriteByte(0);
				g_engfuncs.pfnWriteString("");
				g_engfuncs.pfnMessageEnd();
			}
		}
	}
}

bool CLogMenu::Handle(int EntityIndex, int Key)
{
	if (this->m_Page != -1)
	{
		auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

		if (Player)
		{
			if (Player->m_iMenu == Menu_OFF)
			{
				if (Key == 9)
				{
					this->Display(EntityIndex, ++this->m_Page);
				}
				else if (Key == 10)
				{
					this->Display(EntityIndex, --this->m_Page);
				}
				else
				{
					unsigned int ItemIndex = (Key + (this->m_Page * this->m_PageOption)) - 1;

					if (ItemIndex < this->m_Data.size())
					{
						this->Hide(EntityIndex);

						if (this->m_Func)
						{
							((void(*)(int, std::string Callback, P_MENU_ITEM))this->m_Func)(EntityIndex, this->m_Callback, this->m_Data[ItemIndex]);
						}
					}
				}
			}
			else
			{
				this->m_Page = -1;
			}
		}
	}

	return false;
}

void CLogMenu::Display(int EntityIndex, int Page)
{
	if (Page < 0)
	{
		Page = 0;

		this->m_Page = 0;

		if (this->m_Exit)
		{
			this->m_Page = -1;
			return;
		}
	}
	else
	{
		this->m_Page = Page;
	}

	unsigned int Start = (Page * this->m_PageOption);

	if (Start >= this->m_Data.size())
	{
		Start = Page = this->m_Page = 0;
	}

	auto PageCount = (int)this->m_Data.size() > this->m_PageOption ? (this->m_Data.size() / this->m_PageOption + ((this->m_Data.size() % this->m_PageOption) ? 1 : 0)) : 1;

	std::string MenuText = "";

	MenuText = "\\y" + this->m_Text;

	if (PageCount > 1)
	{
		MenuText += "\\R";
		MenuText += std::to_string(Page + 1);
		MenuText += "/";
		MenuText += std::to_string(PageCount);
	}

	MenuText += "\n\\w\n";

	unsigned int End = (Start + this->m_PageOption);

	if (End > this->m_Data.size())
	{
		End = this->m_Data.size();
	}

	int Slots = MENU_KEY_0; // MENU_KEY_0
	int BitSum = 0;

	for (unsigned int b = Start; b < End; b++)
	{
		Slots |= (1 << BitSum);

		MenuText += "\\r";
		MenuText += std::to_string(++BitSum);
		MenuText += this->m_Data[b].Disabled ? ".\\d " : ".\\w ";
		MenuText += this->m_Data[b].Text;
		MenuText += "\n";
	}

	if (End != this->m_Data.size())
	{
		Slots |= MENU_KEY_9; // MENU_KEY_9;

		if (Page)
		{
			MenuText += "\n\\r9.\\w Next\n\\r0.\\w Back";
		}
		else
		{
			MenuText += "\n\\r9.\\w Next";

			if (this->m_Exit)
			{
				MenuText += "\n\\r0.\\w Exit";
			}
		}
	}
	else
	{
		if (Page)
		{
			MenuText += "\n\\r0.\\w Back";
		}
		else
		{
			if (this->m_Exit)
			{
				MenuText += "\n\\r0.\\w Exit";
			}
		}
	}

	this->ShowMenu(EntityIndex, Slots, -1, MenuText);
}

void CLogMenu::ShowMenu(int EntityIndex, int Slots, int Time, std::string Text)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		if (!Player->IsDormant())
		{
			if (!Player->IsBot())
			{
				static int iMsgShowMenu;

				if (iMsgShowMenu || (iMsgShowMenu = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "ShowMenu", NULL)))
				{
					Player->m_iMenu = Menu_OFF;

					gLogUtil.ReplaceAll(Text, "^w", "\\w");
					gLogUtil.ReplaceAll(Text, "^y", "\\y");
					gLogUtil.ReplaceAll(Text, "^r", "\\r");
					gLogUtil.ReplaceAll(Text, "^R", "\\R");
					gLogUtil.ReplaceAll(Text, "^n", "\n");

					char BufferMenu[MAX_BUFFER_MENU * 6] = { 0 };

					Text.copy(BufferMenu, Text.length() + 1);

					char* pMenuList = BufferMenu;
					char* aMenuList = BufferMenu;

					int iCharCount = 0;

					while (pMenuList && *pMenuList)
					{
						char szChunk[MAX_BUFFER_MENU + 1] = { 0 };

						strncpy(szChunk, pMenuList, MAX_BUFFER_MENU);

						szChunk[MAX_BUFFER_MENU] = 0;

						iCharCount += strlen(szChunk);

						pMenuList = aMenuList + iCharCount;

						g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgShowMenu, nullptr, Player->edict());
						g_engfuncs.pfnWriteShort(Slots);
						g_engfuncs.pfnWriteChar(Time);
						g_engfuncs.pfnWriteByte(*pMenuList ? TRUE : FALSE);
						g_engfuncs.pfnWriteString(szChunk);
						g_engfuncs.pfnMessageEnd();
					}
				}
			}
		}
	}
}
