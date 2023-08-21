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
	this->m_FrameTime = (gpGlobals->time + this->log_api_delay->value);

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
			auto json = nlohmann::json::parse(fp, nullptr, true, true);

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
	catch (nlohmann::json::parse_error& e)
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
					try
					{
						auto Result = nlohmann::ordered_json::parse(Memory, nullptr, true, true);

						if (!Result.empty())
						{
							gLogApi.EventResult(EventIndex, Result);
						}
					}
					catch (nlohmann::ordered_json::parse_error& e)
					{
						LOG_CONSOLE(PLID, "[%s] %s", __func__, e.what());
					}
				}
			}
		}
	}
}

// Parse Event Result
void CLogApi::EventResult(int EventIndex, nlohmann::ordered_json Data)
{
	// Check if has event result from api
	if (Data.contains("ServerExecute"))
	{
		// If is not empty
		if (!Data["ServerExecute"].empty())
		{
			// Check if event result is string
			if (Data["ServerExecute"].is_string())
			{
				// Get Command
				auto String = std::string(Data["ServerExecute"]);

				// If command is not empty
				if (!String.empty())
				{
					// Execute command
					gLogUtil.ServerExecute(String);
				}
			}
			//
			// Check if event result is array
			else if (Data["ServerExecute"].is_array())
			{
				// Loop commands
				for (auto it = Data["ServerExecute"].begin(); it != Data["ServerExecute"].end(); ++it)
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
	}
}

nlohmann::ordered_json CLogApi::GetServerInfo()
{
	nlohmann::ordered_json ServerInfo;

	ServerInfo["Address"] = g_engfuncs.pfnCVarGetString("net_address");

	ServerInfo["Hostname"] = g_engfuncs.pfnCVarGetString("hostname");

	ServerInfo["Map"] = STRING(gpGlobals->mapname);

	ServerInfo["MaxPlayers"] = gpGlobals->maxClients;

	auto Players = gLogPlayer.GetPlayers();

	if (!Players.empty())
	{
		for (auto const& Player : Players)
		{
			ServerInfo[Player.first] =
			{
				{"Auth", Player.first},
				{"Name", Player.second.Name},
				{"Address", Player.second.Address},
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
