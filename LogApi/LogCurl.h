#pragma once

#include <curl/curl.h>
#include <map>
#include <string>

#pragma pack(push, 4)
struct P_CURL_MOD_MEMORY {
    char *Memory;
    size_t Size;
    struct curl_slist *Headers;
    int EventIndex;
};
#pragma pack(pop)

class CLogCurl {
public:
    CLogCurl();
    ~CLogCurl();
    void ServerActivate();
    void ServerFrame();
    void ServerShutdown();
    void PostJSON(const char *url, long timeout, std::string bearerToken, std::string postData, int eventIndex);
    static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

private:
    CURLM *m_MultiHandle;
    long m_RequestIndex;
    std::map<long, P_CURL_MOD_MEMORY> m_Data;
};

extern class CLogCurl* g_pLogCurl;
#define gLogCurl (*g_pLogCurl)