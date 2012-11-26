#ifndef MSGPARSER_H
#define MSGPARSER_H

#include <stdint.h>
#include <vector>
#include <deque>
#include <string>

#include "mpdclient.h"
#include "curlclient.h"

enum msgcode
{
  MSG_START   = 0x55,
  MSG_END     = 0xCC,
  MSG_SKIP    = 0x01,
  MSG_VOLDOWN = 0x02,
  MSG_VOLUP   = 0x03
};

class CMsgParser
{
  public:
    CMsgParser(CMpdClient& mpdclient, CCurlClient& curlclient, int skiptimeout, const char* skipcmd);
    void AddData(uint8_t* data, int size);

  private:
    void Process(uint8_t byte);
    void ProcessMsg();
    void ExecSkipCmd();

    int m_prev;
    int m_skiptimeout;
    int64_t m_lastskiptime;
    std::vector<uint8_t> m_msg;
    CMpdClient&  m_mpdclient;
    CCurlClient& m_curlclient;
    std::string m_skipcmd;
};

#endif //MSGPARSER_H
