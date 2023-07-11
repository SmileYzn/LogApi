#include "precompiled.h"

CLogApi gLogApi;

// On Server Activate Event
void CLogApi::ServerActivate()
{
	// Enable Log API (0 Disable, 1 Enable)
	this->m_log_api_on = gLogUtil.CvarRegister("log_api_enable", "1");

	// Set Log API Address (API HTTP/s Address Ie. https://api.yoursite.com/)
	this->m_log_api_address = gLogUtil.CvarRegister("log_api_address", "");

	// Set Log API Timeout (Timeout in seconds to wait for response from remote server)
	this->m_log_api_timeout = gLogUtil.CvarRegister("log_api_timeout", "5.0");

	// Set Log API Bearer Token (Authentication Token or leave empty to disable)
	this->m_log_api_bearer = gLogUtil.CvarRegister("log_api_bearer", "");

	// Clear events
	this->m_Events.clear();

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
				this->m_Events.insert(std::make_pair(event.key(), event.value().get<bool>()));
			}
		}
		else
		{
			LOG_CONSOLE(PLID, "[%s] Failed to open file: %s", __func__, LOG_API_FILE_EVENTS);
		}
	}
	catch (nlohmann::json::parse_error& e)
	{
		LOG_CONSOLE(PLID, "[%s] %s", __func__, e.what());
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
		// Check if event result name is not empty and is string
		if (Data["ServerExecute"].is_string())
		{
			// Get Command
			auto String = Data["ServerExecute"].get<std::string>();

			// If command is not empty
			if (!String.empty())
			{
				// Execute command
				gLogUtil.ServerExecute(String);
			}
		}
	}
}
