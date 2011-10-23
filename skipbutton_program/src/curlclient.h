#ifndef CURLCLIENT_H
#define CURLCLIENT_H

#include <curl/curl.h>

#include "util/thread.h"
#include "util/condition.h"

class CCurlClient : public CThread
{
  public:
    CCurlClient();
    ~CCurlClient();

    void Skip();
    void Stop();
    
  private:
    void         Process();

    CURL*        m_handle;
    curl_slist*  m_headers;
    CCondition   m_condition;
    unsigned int m_skipcount;
};

#endif //CURLCLIENT_H
