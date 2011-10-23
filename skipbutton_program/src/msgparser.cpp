#include <cstdio>
#include "msgparser.h"

CMsgParser::CMsgParser(CMpdClient& mpdclient, CCurlClient& curlclient): m_mpdclient(mpdclient), m_curlclient(curlclient)
{
  m_prev = -1;
}

void CMsgParser::AddData(uint8_t* data, int size)
{
  for (int i = 0; i < size; i++)
    Process(data[i]);
}

void CMsgParser::Process(uint8_t byte)
{
  if (m_prev == -1)
  {
    m_prev = byte;
    return;
  }

  uint8_t prev = ~m_prev;
  if (byte == prev)
  {
    m_msg.push_back(m_prev);
    ProcessMsg();
    m_prev = -1;
  }
  else
  {
    m_msg.clear();
    m_prev = byte;
  }
}

void CMsgParser::ProcessMsg()
{
  if (m_msg[0] != (uint8_t)MSG_START || (m_msg.size() == 3 && m_msg[2] != (uint8_t)MSG_END))
  {
    m_msg.clear();
    return;
  }

  if (m_msg.size() == 3)
  {
    if (m_msg[1] == MSG_SKIP)
    {
      printf("skip\n");
      m_curlclient.Skip();
    }
    else if (m_msg[1] == MSG_VOLUP)
    {
      printf("volup\n");
      m_mpdclient.VolumeUp();
    }
    else if (m_msg[1] == MSG_VOLDOWN)
    {
      printf("voldown\n");
      m_mpdclient.VolumeDown();
    }

    m_msg.clear();
  }
}

