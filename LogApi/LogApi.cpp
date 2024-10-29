
#include "precompiled.h"

CLogApi gLogApi;

// On server activate event
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
	this->m_log_api_delay = gLogUtil.CvarRegister("log_api_delay", "60.0");

	// Execute Settings File
	g_engfuncs.pfnServerCommand("exec addons/logapi/logapi.cfg\n");

	// Plugin is running
	this->m_Running = true;

	// Reset next frame time
	this->m_FrameTime = (gpGlobals->time + this->m_log_api_delay->value);

	try
	{
		// File stream
		FILE* fp = fopen(LOG_API_FILE_EVENTS, "r");

		if (fp)
		{
			// Set file pointer to end of file
			fseek(fp, 0, SEEK_END);

			// Get final position
			long fs = ftell(fp);

			// Set file pointer to start of file
			fseek(fp, 0, SEEK_SET);

			// Create empty std::string with file size
			std::string buffer(fs, '\0');

			// Read file to std::string buffer
			fread(&buffer[0], 1, fs, fp);

			// Close file pointer
			fclose(fp);

			// Read data
			auto json = nlohmann::ordered_json::parse(buffer, nullptr, true, true);

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

// On server frame
void CLogApi::ServerFrame()
{
	// If variable is not null
	if (this->m_log_api_delay)
	{
		// If is set more than 5 seconds
		if (this->m_log_api_delay->value > 5.0f)
		{
			// If frame time was passed
			if (gpGlobals->time >= this->m_FrameTime)
			{
				// Is Running?
				if (this->m_Running)
				{
					// Send server info
					gLogEvent.ServerInfo();
	
					// Set next frame time
					this->m_FrameTime = (gpGlobals->time + this->m_log_api_delay->value);
				}
			}
		}
	}
}

// Check if event is enabled
int CLogApi::EventEnabled(const char* EventName)
{
	// If has event name
	if (EventName)
	{
		// If log api is enabled
		if (this->m_log_api_on->value)
		{
			// If log api has target address
			if (this->m_log_api_address->string)
			{
				// Find event by name
				if (this->m_Events.find(EventName) != this->m_Events.end())
				{
					// Retorn enable / disabled
					return this->m_Events[EventName];
				}
			}
		}
	}

	// Return 0
	return 0;
}

// Send event
void CLogApi::SendEvent(int EventIndex, nlohmann::ordered_json Event)
{
	if (this->m_log_api_address)
	{
		// If log api is enabled
		if (this->m_log_api_on->value)
		{
			// If log api has target address
			if (this->m_log_api_address->string)
			{
				// If JSON is not empty
				if (!Event.empty())
				{
					if (this->m_log_api_timeout)
					{
						if (this->m_log_api_bearer)
						{
							// POST to webserver
							gLogCurl.PostJSON(this->m_log_api_address->string, (long)this->m_log_api_timeout->value, this->m_log_api_bearer->string, Event.dump(), (void*)this->CallbackResult, EventIndex);
						}
					}
				}
			}
		}
	}
}

// Callback
void CLogApi::CallbackResult(CURL* ch, size_t Size, const char* Memory, int EventIndex)
{
	if (ch)
	{
		// HTTP Status Code
		long HttpResponseCode = 0;

		// If can get info
		if (curl_easy_getinfo(ch, CURLINFO_RESPONSE_CODE, &HttpResponseCode) == CURLE_OK)
		{
			// If response code is ok
			if (HttpResponseCode == 200)
			{
				// If memory is not null
				if (Memory)
				{
					// If is not empty
					if (Memory[0u] != '\0')
					{
						try
						{
							// Parse json data to result
							auto Result = nlohmann::ordered_json::parse(Memory, nullptr, true, true);

							// If is not empty
							if (!Result.empty())
							{
								// If has data size
								if (Result.size() > 0)
								{
									// Call event result
									gLogApi.EventResult(EventIndex, Result);
								}
							} 
						}
						catch (const nlohmann::ordered_json::parse_error& e)
						{
							// Log
							LOG_CONSOLE(PLID, "[%s] %s", __func__, e.what());
						}
					}
				}
			}
			else
			{
				// Log
				LOG_CONSOLE(PLID, "[%s] Unknow response from server: HTTP Code %d, check log_api_address.", __func__, HttpResponseCode);
			}
		}

		// Clear
		curl_easy_cleanup(ch);
	}
}

// Parse event result
void CLogApi::EventResult(int EventIndex, nlohmann::ordered_json Data)
{
	// Check if has event 'ServerCommand' result from api
	if (Data.contains("ServerCommand"))
	{
		this->ServerCommand(EventIndex, Data);
	}

	// Check if has event 'ShowMenu' result from api
	if (Data.contains("ShowMenu"))
	{
		this->ShowMenu(EventIndex, Data);
	}

	// Check if has event 'ClientPrint' result from api
	if (Data.contains("ClientPrint"))
	{
		this->ClientPrint(EventIndex, Data);
	}

	// Check if has event 'PrintChat' result from api
	if (Data.contains("PrintChat"))
	{
		this->PrintChat(EventIndex, Data);
	}

	// Check if has event 'ShowHudMessage' result from api
	if (Data.contains("ShowHudMessage"))
	{
		this->ShowHudMessage(EventIndex, Data);
	}
}

// Execute server command from result
void CLogApi::ServerCommand(int EventIndex, nlohmann::ordered_json Data)
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
void CLogApi::ShowMenu(int EventIndex, nlohmann::ordered_json Data)
{
	if (!Data[__func__].empty())
	{
		if (Data[__func__].is_object())
		{
			if (!Data[__func__]["Items"].empty())
			{
				if (Data[__func__]["Items"].is_array())
				{
					try
					{
						auto Title = Data[__func__]["Title"].get<std::string>();

						auto Exit = Data[__func__]["Exit"].get<bool>();

						auto Callback = Data[__func__]["Callback"].get<std::string>();

						if (Data[__func__]["EntityId"].is_number_integer())
						{
							auto EntityId = Data[__func__]["EntityId"].get<int>();

							if (EntityId)
							{
								this->Menu(EntityId, Title, Exit, Callback, Data[__func__]["Items"]);
							}
							else
							{
								auto Players = gLogUtil.GetPlayers();

								if (!Players.empty())
								{
									for (auto Player : Players)
									{
										this->Menu(Player->entindex(), Title, Exit, Callback, Data[__func__]["Items"]);
									}
								}
							}
						}
						else if (Data[__func__]["EntityId"].is_array())
						{
							for (auto it = Data[__func__]["EntityId"].begin(); it != Data[__func__]["EntityId"].end(); ++it)
							{
								if (it.value().is_number_integer())
								{
									this->Menu(it.value().get<int>(), Title, Exit, Callback, Data[__func__]["Items"]);
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

// Print to client from result
void CLogApi::ClientPrint(int EventIndex, nlohmann::ordered_json Data)
{
	// If is not empty
	if (!Data[__func__].empty())
	{
		try
		{
			if (Data[__func__].is_object())
			{
				// If is not empty
				if (!Data[__func__]["EntityId"].empty() && !Data[__func__]["PrintType"].empty() && !Data[__func__]["Message"].empty())
				{
					// Entity index
					auto EntityId = Data[__func__]["EntityId"].get<int>();

					// Print Type
					auto PrintType = Data[__func__]["PrintType"].get<int>();

					// Get message string
					auto Message = Data[__func__]["Message"].get<std::string>();

					// Entity pointer
					edict_t* pEntity = nullptr;

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
						gLogUtil.ClientPrint(pEntity, PrintType, "%s", Message.c_str());
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
void CLogApi::PrintChat(int EventIndex, nlohmann::ordered_json Data)
{
	// If is not empty
	if (!Data[__func__].empty())
	{
		try
		{
			if (Data[__func__].is_object())
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
						gLogUtil.SayText(pEntity, ENTINDEX(pEntity), Message.c_str());
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

// Print to hudmessage chat from result
void CLogApi::ShowHudMessage(int EventIndex, nlohmann::ordered_json Data)
{
	// If is not empty
	if (!Data[__func__].empty())
	{
		try
		{
			if (Data[__func__].is_object())
			{
				// If is not empty
				if (!Data[__func__]["EntityId"].empty() && !Data[__func__]["TeamSay"].empty() && !Data[__func__]["Message"].empty())
				{
					// Entity index
					auto EntityId = Data[__func__]["EntityId"].get<int>();

					// TeamSay (false Center message, true Left message)
					auto TeamSay = Data[__func__]["TeamSay"].get<bool>();

					// Get message string
					auto Message = Data[__func__]["Message"].get<std::string>();

					// Entity pointer
					edict_t* pEntity = nullptr;

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
						gLogUtil.HudMessage(pEntity, gLogCommand.GetHudParameters(TeamSay), "%s", Message.c_str());
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
	// Json
	nlohmann::ordered_json ServerInfo;

	// Set address
	ServerInfo["Address"] = g_engfuncs.pfnCVarGetString("net_address");

	// Set hostname
	ServerInfo["Hostname"] = g_engfuncs.pfnCVarGetString("hostname");

	// Set map name
	ServerInfo["Map"] = STRING(gpGlobals->mapname);

	// Set game
	ServerInfo["Game"] = "Counter-Strike";

	// If CSGameRules is not null
	if (g_pGameRules)
	{
		// If server has game description
		if (CSGameRules()->m_GameDesc)
		{
			// If is not empty
			if (CSGameRules()->m_GameDesc[0u] != '\0')
			{
				// Set game description name
				ServerInfo["Game"] = CSGameRules()->m_GameDesc;
			}
		}
	}

	// Set max players
	ServerInfo["MaxPlayers"] = gpGlobals->maxClients;

	// Players
	auto Players = gLogPlayer.GetPlayers();

	// If player list is not empty
	if (!Players.empty())
	{
		// Loop players
		for (auto const& Player : Players)
		{
			// Set player information
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

	// Return JSON data
	return ServerInfo;
}
