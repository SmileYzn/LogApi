#pragma once

struct P_CURL_MOD_MEMORY
{
	char* Memory;
	size_t Size;
	int EventIndex;
};

class CLogCurl
{
public:
	void ServerActivate();
	void ServerFrame();

	void PostJSON(const char* url, long Timeout, std::string BearerToken, std::string PostData, int EventIndex);

	static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);

private:
	CURLM* m_MultiHandle = NULL;

	long m_RequestIndex = 0;

	std::map<long, P_CURL_MOD_MEMORY> m_Data;
};

extern CLogCurl gLogCurl;
