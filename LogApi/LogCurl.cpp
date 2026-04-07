#include "precompiled.h"

CLogCurl gLogCurl;

void CLogCurl::ServerActivate() {
  if (!this->m_MultiHandle) {
    this->m_RequestIndex = 0;

    this->m_Data.clear();

    curl_global_init(CURL_GLOBAL_ALL);

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

        curl_multi_remove_handle(this->m_MultiHandle, MsgInfo->easy_handle);

        curl_easy_cleanup(MsgInfo->easy_handle);

        ProcessedThisFrame++;
      }
    }
  }
}

void CLogCurl::PostJSON(const char *url, long Timeout, std::string BearerToken,
                        std::string PostData, int EventIndex) {
  if (this->m_MultiHandle) {
    if (url) {
      CURL *ch = curl_easy_init();

      if (ch) {
        this->m_Data[this->m_RequestIndex] = {0};

        this->m_Data[this->m_RequestIndex].EventIndex = EventIndex;
        this->m_Data[this->m_RequestIndex].Headers = NULL;

        curl_easy_setopt(ch, CURLOPT_URL, url);

        curl_easy_setopt(ch, CURLOPT_TIMEOUT, (Timeout) > 0 ? Timeout : 5);

        curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1L);

        curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, this->WriteMemoryCallback);

        curl_easy_setopt(ch, CURLOPT_NOPROGRESS, 1L);

        curl_easy_setopt(ch, CURLOPT_POST, 1L);

        curl_easy_setopt(ch, CURLOPT_POSTFIELDSIZE, (long)PostData.size());

        curl_easy_setopt(ch, CURLOPT_COPYPOSTFIELDS, PostData.c_str());

        struct curl_slist *chHeaders =
            curl_slist_append(NULL, "Content-Type: application/json");

        if (BearerToken.length() > 0) {
          std::string AuthorizationHeader =
              "Authorization: Bearer " + BearerToken;

          chHeaders = curl_slist_append(chHeaders, AuthorizationHeader.c_str());
        }

        curl_easy_setopt(ch, CURLOPT_HTTPHEADER, chHeaders);

        this->m_Data[this->m_RequestIndex].Headers = chHeaders;

        curl_easy_setopt(ch, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(ch, CURLOPT_SSL_VERIFYHOST, 2L);

        curl_easy_setopt(ch, CURLOPT_WRITEDATA,
                         (void *)&this->m_Data[this->m_RequestIndex]);

        curl_easy_setopt(ch, CURLOPT_PRIVATE, (void*)(intptr_t)this->m_RequestIndex);

        curl_multi_add_handle(this->m_MultiHandle, ch);

        if (this->m_RequestIndex >= LONG_MAX) {
          this->m_RequestIndex = 1;
        }

        this->m_RequestIndex++;
      }
    }
  }
}

size_t CLogCurl::WriteMemoryCallback(void *contents, size_t size, size_t nmemb,
                                     void *userp) {
  if (contents) {
    if (userp) {
      size_t realsize = size * nmemb;

      if (realsize > 0) {
        P_CURL_MOD_MEMORY *mem = (P_CURL_MOD_MEMORY *)userp;

        char *ptr = (char *)realloc(mem->Memory, mem->Size + realsize + 1);

        if (ptr) {
          mem->Memory = ptr;

          memcpy(&(mem->Memory[mem->Size]), contents, realsize);

          mem->Size += realsize;

          mem->Memory[mem->Size] = 0;

          return realsize;
        }
      }
    }
  }

  return 0;
}
