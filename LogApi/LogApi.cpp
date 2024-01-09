#include "precompiled.h"

CLogApi gLogApi;

// On Server Activate Event
void CLogApi::ServerActivate()
{
	// Enable Log API (0 Disable, 1 Enable)
	this->m_log_api_on = gLogUtil.CvarRegister("log_api_enable", "0");

	// Set Log API Address (API HTTP/s Address Ie. https://api.yoursite.com/)
	this->m_log_api_address = gLogUtil.CvarRegister("log_api_address", "");

	// Set Log API Timeout (Timeout in seconds to wait for response from remote server)
	this->m_log_api_timeout = gLogUtil.CvarRegister("log_api_timeout", "5.0");

	// Set Log API Bearer Token (Authentication Token or leave empty to disable)
	this->m_log_api_bearer = gLogUtil.CvarRegister("log_api_bearer", "");

	// Server Info Event Delay (Delay to update Server Info on webserver)
	this->log_api_delay = gLogUtil.CvarRegister("log_api_delay", "60.0");

	// Execute Settings File
	g_engfuncs.pfnServerCommand("exec addons/logapi/logapi.cfg\n");

	// Clear events
	this->m_Events.clear();

	// Frame Time
	this->m_FrameTime = 0.0f;

	try
	{
		// File stream
		std::ifstream fp(LOG_API_FILE_EVENTS);

		if (fp)
		{
			// Reset pointer
			fp.clear();

			// Set pointer to begin of file
			fp.seekg(0, std::ios::beg);

			// Read data
			auto json = nlohmann::ordered_json::parse(fp, nullptr, true, true);

			// Loop each item
			for (auto const& event : json.items())
			{
				// Insert event as enabled / disabled
				this->m_Events.insert(std::make_pair(event.key(), event.value().get<bool>()));
			}
		}
		else
		{
			// Failed on error
			LOG_CONSOLE(PLID, "[%s] Failed to open file: %s", __func__, LOG_API_FILE_EVENTS);
		}
	}
	catch (const nlohmann::ordered_json::parse_error& e)
	{
		// JSON exeption errors
		LOG_CONSOLE(PLID, "[%s] %s", __func__, e.what());
	}
}

// On Server Frame
void CLogApi::ServerFrame()
{
	if (this->log_api_delay)
	{
		if (this->log_api_delay->value > 5.0f)
		{
			if (gpGlobals->time >= this->m_FrameTime)
			{
				gLogEvent.ServerInfo();

				this->m_FrameTime = (gpGlobals->time + this->log_api_delay->value);
			}
		}
	}
}

// Check if event is enabled
bool CLogApi::EventEnabled(const char* EventName)
{
	if (EventName)
	{
		if (this->m_log_api_on->value)
		{
			if (this->m_log_api_address->string)
			{
				if (this->m_Events.find(EventName) != this->m_Events.end())
				{
					return this->m_Events[EventName];
				}
			}
		}
	}

	return false;
}

// Send Event
void CLogApi::SendEvent(int EventIndex, nlohmann::ordered_json Event)
{
	if (this->m_log_api_on->value)
	{
		if (this->m_log_api_address->string)
		{
			if (!Event.empty())
			{
				gLogCurl.PostJSON(this->m_log_api_address->string, (long)this->m_log_api_timeout->value, this->m_log_api_bearer->string, Event.dump(), (void*)this->CallbackResult, EventIndex);
			}
		}
	}
}

// Callback
void CLogApi::CallbackResult(CURL* ch, size_t Size, const char* Memory, int EventIndex)
{
	if (ch)
	{
		long HttpResponseCode = 0;

		if (curl_easy_getinfo(ch, CURLINFO_RESPONSE_CODE, &HttpResponseCode) == CURLE_OK)
		{
			if (HttpResponseCode == 200)
			{
				if (Memory)
				{
					if (Memory[0u] != '\0')
					{
						try
						{
							auto Result = nlohmann::ordered_json::parse(Memory, nullptr, true, true);

							if (!Result.empty())
							{
								if (Result.size() > 0)
								{
									gLogApi.EventResult(EventIndex, Result);
								}
							} 
						}
						catch (const nlohmann::ordered_json::parse_error& e)
						{
							LOG_CONSOLE(PLID, "[%s] %s", __func__, e.what());
						}
					}
				}
			}
			else
			{
				LOG_CONSOLE(PLID, "[%s] Unknow response from server: HTTP Code %d, check log_api_address.", __func__, HttpResponseCode);
			}
		}
	}
}

// Parse Event Result
void CLogApi::EventResult(int EventIndex, nlohmann::ordered_json Data)
{
	// Check if has event 'ServerExecute' result from api
	if (Data.contains("ServerExecute"))
	{
		this->ServerExecute(EventIndex, Data);
	}

	// Check if has event 'ServerMenu' result from api
	if (Data.contains("ServerMenu"))
	{
		this->ServerMenu(EventIndex, Data);
	}

	// Check if has event 'ServerConsoleLog' result from api
	if (Data.contains("ServerConsoleLog"))
	{
		this->ServerConsoleLog(EventIndex, Data);
	}

	if (Data.contains("ServerSay"))
	{
		this->ServerSay(EventIndex, Data);
	}
}

// Execute server command from result
void CLogApi::ServerExecute(int EventIndex, nlohmann::ordered_json Data)
{
	// If is not empty
	if (!Data[__func__].empty())
	{
		try
		{
			// Check if event result is string
			if (Data[__func__].is_string())
			{
				// Get Command
				auto String = Data[__func__].get<std::string>();

				// If command is not empty
				if (!String.empty())
				{
					// Execute command
					gLogUtil.ServerExecute(String);
				}
			}
			//
			// Check if event result is array
			else if (Data[__func__].is_array())
			{
				// Loop commands
				for (auto it = Data[__func__].begin(); it != Data[__func__].end(); ++it)
				{
					// If is strimg
					if (it.value().is_string())
					{
						// If is not empty
						if (!it.value().empty())
						{
							// Get string of this command
							auto String = std::string(it.value());

							// If is not empty
							if (!String.empty())
							{
								// Execute command
								gLogUtil.ServerExecute(String);
							}
						}
					}
				}
			}
		}
		catch (const nlohmann::ordered_json::exception& e)
		{
			LOG_CONSOLE(PLID, "[%s] %s", __func__, e.what());
		}
	}
}

// Open menu from result
void CLogApi::ServerMenu(int EventIndex, nlohmann::ordered_json Data)
{
	if (!Data["ServerMenu"].empty())
	{
		if (Data["ServerMenu"].is_object())
		{
			if (!Data["ServerMenu"]["Items"].empty())
			{
				if (Data["ServerMenu"]["Items"].is_array())
				{
					try
					{
						auto Title = Data["ServerMenu"]["Title"].get<std::string>();

						auto Exit = Data["ServerMenu"]["Exit"].get<bool>();

						auto Callback = Data["ServerMenu"]["Callback"].get<std::string>();

						if (Data["ServerMenu"]["EntityId"].is_number_integer())
						{
							auto EntityId = Data["ServerMenu"]["EntityId"].get<int>();

							if (EntityId)
							{
								this->Menu(EntityId, Title, Exit, Callback, Data["ServerMenu"]["Items"]);
							}
							else
							{
								auto Players = gLogUtil.GetPlayers();

								if (!Players.empty())
								{
									for (auto Player : Players)
									{
										this->Menu(Player->entindex(), Title, Exit, Callback, Data["ServerMenu"]["Items"]);
									}
								}
							}
						}
						else if (Data["ServerMenu"]["EntityId"].is_array())
						{
							for (auto it = Data["ServerMenu"]["EntityId"].begin(); it != Data["ServerMenu"]["EntityId"].end(); ++it)
							{
								if (it.value().is_number_integer())
								{
									this->Menu(it.value().get<int>(), Title, Exit, Callback, Data["ServerMenu"]["Items"]);
								}
							}
						}
					}
					catch (const nlohmann::ordered_json::exception& e)
					{
						LOG_CONSOLE(PLID, "[%s] %s", __func__, e.what());
					}
				}
				else
				{
					LOG_CONSOLE(PLID, "[%s] Menu is empty", __func__);
				}
			}
		}
	}
}

// Open menu function
void CLogApi::Menu(int EntityIndex, std::string Title, bool Exit, std::string Callback, nlohmann::ordered_json Items)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		if (!Player->IsBot())
		{
			if (Player->IsPlayer())
			{
				gLogMenu[EntityIndex].Create(Title, Callback, Exit, (void*)gLogApi.MenuHandle);

				for (auto it = Items.begin(); it != Items.end(); ++it)
				{
					auto Option = it.value();

					if (Option.is_object())
					{
						if (!Option.empty())
						{
							auto Info = Option["Info"].get<std::string>();

							auto Text = Option["Text"].get<std::string>();

							auto Disabled = Option["Disabled"].get<bool>();

							auto Extra = Option["Extra"].get<std::string>();

							gLogMenu[EntityIndex].AddItem(Info, Text, Disabled, Extra);
						}
					}
				}

				gLogMenu[EntityIndex].Show(EntityIndex);
			}
		}
	}
}

// Open menu function handler
void CLogApi::MenuHandle(int EntityIndex, std::string Callback, P_MENU_ITEM Item)
{
	gLogEvent.ClientMenuHandle(INDEXENT(EntityIndex), Callback, Item);
}

// Print to server console from result
void CLogApi::ServerConsoleLog(int EventIndex, nlohmann::ordered_json Data)
{
	// If is not empty
	if (!Data[__func__].empty())
	{
		try
		{
			// Check if event result is string
			if (Data[__func__].is_string())
			{
				// Get Command
				auto Message = Data[__func__].get<std::string>();

				// If command is not empty
				if (!Message.empty())
				{
					// Execute command
					gLogUtil.ClientPrint(nullptr, PRINT_CONSOLE, "%s", Message.c_str());
				}
			}
			//
			// Check if event result is array
			else if (Data[__func__].is_object())
			{
				// If is not empty
				if (!Data[__func__]["EntityId"].empty() && !Data[__func__]["Message"].empty())
				{
					// Entity pointer
					edict_t* pEntity = nullptr;

					// Entity index
					auto EntityId = Data[__func__]["EntityId"].get<int>();

					// Get message string
					auto Message = Data[__func__]["Message"].get<std::string>();

					// If has entity index
					if (EntityId > 0)
					{
						// Get entity pointer
						pEntity = FNullEnt(INDEXENT(EntityId)) ? INDEXENT(EntityId) : nullptr;
					}

					// If is not empty
					if (!Message.empty())
					{
						// Print to entity
						gLogUtil.ClientPrint(pEntity, PRINT_CONSOLE, "%s", Message.c_str());
					}
				}
			}
		}
		catch (const nlohmann::ordered_json::exception& e)
		{
			LOG_CONSOLE(PLID, "[%s] %s", __func__, e.what());
		}
	}
}

// Print to player chat from result
void CLogApi::ServerSay(int EventIndex, nlohmann::ordered_json Data)
{
	// If is not empty
	if (!Data[__func__].empty())
	{
		try
		{
			// Check if event result is string
			if (Data[__func__].is_string())
			{
				// Get Command
				auto Message = Data[__func__].get<std::string>();

				// If command is not empty
				if (!Message.empty())
				{
					// Execute command
					gLogUtil.SayText(nullptr, PRINT_TEAM_DEFAULT, "%s", Message.c_str());
				}
			}
			//
			// Check if event result is array
			else if (Data[__func__].is_object())
			{
				// If is not empty
				if (!Data[__func__]["EntityId"].empty() && !Data[__func__]["Message"].empty())
				{
					// Entity pointer
					edict_t* pEntity = nullptr;

					// Entity index
					auto EntityId = Data[__func__]["EntityId"].get<int>();

					// Get message string
					auto Message = Data[__func__]["Message"].get<std::string>();

					// If has entity index
					if (EntityId > 0)
					{
						// Get entity pointer
						pEntity = FNullEnt(INDEXENT(EntityId)) ? INDEXENT(EntityId) : nullptr;
					}

					// If is not empty
					if (!Message.empty())
					{
						// Print to entity
						gLogUtil.SayText(pEntity, ENTINDEX(pEntity), "%s", Message.c_str());
					}
				}
			}
		}
		catch (const nlohmann::ordered_json::exception& e)
		{
			LOG_CONSOLE(PLID, "[%s] %s", __func__, e.what());
		}
	}
}

// Get server info as JSON data
nlohmann::ordered_json CLogApi::GetServerInfo()
{
	nlohmann::ordered_json ServerInfo;

	ServerInfo["Address"] = g_engfuncs.pfnCVarGetString("net_address");

	ServerInfo["Hostname"] = g_engfuncs.pfnCVarGetString("hostname");

	ServerInfo["Map"] = STRING(gpGlobals->mapname);

	ServerInfo["Game"] = "Counter-Strike";

	if (g_pGameRules)
	{
		if (CSGameRules()->m_GameDesc)
		{
			if (CSGameRules()->m_GameDesc[0u] != '\0')
			{
				ServerInfo["Game"] = CSGameRules()->m_GameDesc;
			}
		}
	}

	ServerInfo["MaxPlayers"] = gpGlobals->maxClients;

	if (!gLogPlayer.m_Players.empty())
	{
		for (auto const& Player : gLogPlayer.m_Players)
		{
			ServerInfo["Players"][Player.first] =
			{
				{"EntityId", Player.second.EntityId},
				{"Auth", Player.first},
				{"Name", Player.second.Name},
				{"Address", Player.second.Address},
				{"UserId", Player.second.UserId},
				{"Team", Player.second.Team},
				{"Frags", Player.second.Frags},
				{"Deaths", Player.second.Deaths},
				{"GameTime", Player.second.GameTime},
				{"ConnectTime", Player.second.ConnectTime}
			};
		}
	}

	return ServerInfo;
}
