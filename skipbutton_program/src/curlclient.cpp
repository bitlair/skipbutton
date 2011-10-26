#include "curlclient.h"
#include "util/lock.h"

#include <cstring>

CCurlClient::CCurlClient(const char* url)
{
  curl_global_init(CURL_GLOBAL_ALL);

  m_handle  = curl_easy_init();
  m_headers = curl_slist_append(NULL, "Content-type:application/json");

  const char* binaryptr = "{\"action\":\"next\"}";
  curl_easy_setopt(m_handle, CURLOPT_POSTFIELDS, binaryptr);
  curl_easy_setopt(m_handle, CURLOPT_POSTFIELDSIZE, strlen(binaryptr));

  curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, m_headers);

  curl_easy_setopt(m_handle, CURLOPT_URL, url); 

  m_skipcount = 0;
}

CCurlClient::~CCurlClient()
{
  curl_slist_free_all(m_headers);
  curl_easy_cleanup(m_handle);
  curl_global_cleanup();
}

void CCurlClient::Skip()
{
  CLock lock(m_condition);
  m_skipcount++;
  m_condition.Signal();
}

void CCurlClient::Process()
{
  CLock lock(m_condition);
  while (!m_stop)
  {
    while (m_skipcount == 0 && !m_stop)
      m_condition.Wait();

    while (m_skipcount > 0)
    {
      lock.Leave();
      curl_easy_perform(m_handle);
      lock.Enter();

      m_skipcount--;
    }
  }
}

void CCurlClient::Stop()
{
  AsyncStopThread();

  CLock lock(m_condition);
  m_condition.Signal();
  lock.Leave();

  StopThread();
}

