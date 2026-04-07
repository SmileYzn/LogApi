#include "precompiled.h"

// Static pointer to the instance
static CLogCurl* g_pLogCurlInstance = nullptr;

CLogCurl* GetLogCurl() {
    if (g_pLogCurlInstance == nullptr) {
        g_pLogCurlInstance = new CLogCurl();
    }
    return g_pLogCurlInstance;
}

CLogCurl::CLogCurl() {
    m_MultiHandle = nullptr;
    m_RequestIndex = 0;
    m_Data.clear();
}

CLogCurl::~CLogCurl() {
    this->ServerShutdown();
}

void CLogCurl::ServerActivate() {
    if (this->m_MultiHandle == nullptr) {
        // Global cURL initialization
        if (curl_global_init(CURL_GLOBAL_NOTHING) != CURLE_OK) {
            return;
        }
        
        this->m_MultiHandle = curl_multi_init();
    }
}

void CLogCurl::ServerFrame() {
  if (this->m_MultiHandle) {
    int HandleCount = 0;
    int ProcessedThisFrame = 0;

    CURLMsg *MsgInfo = NULL;

    curl_multi_perform(this->m_MultiHandle, &HandleCount);

    while (ProcessedThisFrame < 5 && (MsgInfo = curl_multi_info_read(
                                          this->m_MultiHandle, &HandleCount))) {
      if (MsgInfo->msg == CURLMSG_DONE) {
        char *pPrivate = nullptr;
        curl_easy_getinfo(MsgInfo->easy_handle, CURLINFO_PRIVATE, &pPrivate);
        long Index = (long)(intptr_t)pPrivate;

        if (this->m_Data.find(Index) != this->m_Data.end()) {
          gLogApi.CallbackResult(MsgInfo->easy_handle, this->m_Data[Index].Size,
                                 this->m_Data[Index].Memory,
                                 this->m_Data[Index].EventIndex);

          free(this->m_Data[Index].Memory);

          if (this->m_Data[Index].Headers) {
            curl_slist_free_all(this->m_Data[Index].Headers);
          }

          this->m_Data.erase(Index);
        }
    }
}

void CLogCurl::PostJSON(const char *url, long timeout, std::string bearerToken,
                        std::string postData, int eventIndex) {
    if (!this->m_MultiHandle || !url) return;

    CURL *ch = curl_easy_init();
    if (ch) {
        this->m_RequestIndex++;
        if (this->m_RequestIndex >= LONG_MAX) this->m_RequestIndex = 1;

        // Safety cleanup in case the index already exists
        if (this->m_Data.count(this->m_RequestIndex)) {
            if (this->m_Data[this->m_RequestIndex].Memory) free(this->m_Data[this->m_RequestIndex].Memory);
            if (this->m_Data[this->m_RequestIndex].Headers) curl_slist_free_all(this->m_Data[this->m_RequestIndex].Headers);
        }

        P_CURL_MOD_MEMORY &reqData = this->m_Data[this->m_RequestIndex];
        reqData.EventIndex = eventIndex;
        reqData.Memory = (char*)malloc(1); 
        reqData.Size = 0;
        reqData.Headers = nullptr;

        if (reqData.Memory) reqData.Memory[0] = '\0';

        // --- EASY HANDLE SETUP ---
        curl_easy_setopt(ch, CURLOPT_URL, url);
        curl_easy_setopt(ch, CURLOPT_TIMEOUT, (timeout > 0) ? timeout : 5L);
        curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, this->WriteMemoryCallback);
        curl_easy_setopt(ch, CURLOPT_WRITEDATA, (void *)&reqData);
        curl_easy_setopt(ch, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(ch, CURLOPT_POST, 1L);
        
        // ESSENTIAL: Prevents cURL from using system signals (avoids SIGSEGV in HLDS)
        curl_easy_setopt(ch, CURLOPT_NOSIGNAL, 1L);

        // Copy post data so cURL does not read stale memory
        curl_easy_setopt(ch, CURLOPT_COPYPOSTFIELDS, postData.c_str());

        // Headers
        struct curl_slist *headers = curl_slist_append(nullptr, "Content-Type: application/json");
        if (!bearerToken.empty()) {
            std::string auth = "Authorization: Bearer " + bearerToken;
            headers = curl_slist_append(headers, auth.c_str());
        }
        curl_easy_setopt(ch, CURLOPT_HTTPHEADER, headers);
        reqData.Headers = headers;

        // SSL
        curl_easy_setopt(ch, CURLOPT_SSL_VERIFYPEER, 0L); // Change to 1L in production
        curl_easy_setopt(ch, CURLOPT_SSL_VERIFYHOST, 0L); // Change to 2L in production

        // Request identifier
        curl_easy_setopt(ch, CURLOPT_PRIVATE, (void*)(intptr_t)this->m_RequestIndex);

        curl_multi_add_handle(this->m_MultiHandle, ch);
    }
}

size_t CLogCurl::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    P_CURL_MOD_MEMORY *mem = (P_CURL_MOD_MEMORY *)userp;
    if (!mem) return 0;

    char *ptr = (char *)realloc(mem->Memory, mem->Size + realsize + 1);
    if (ptr == nullptr) return 0;

    mem->Memory = ptr;
    memcpy(&(mem->Memory[mem->Size]), contents, realsize);
    mem->Size += realsize;
    mem->Memory[mem->Size] = 0;

    return realsize;
}

void CLogCurl::ServerShutdown() {
    if (this->m_MultiHandle) {
        for (auto &item : m_Data) {
            if (item.second.Memory) free(item.second.Memory);
            if (item.second.Headers) curl_slist_free_all(item.second.Headers);
        }
        m_Data.clear();

        curl_multi_cleanup(this->m_MultiHandle);
        this->m_MultiHandle = nullptr;
        curl_global_cleanup();
    }
}