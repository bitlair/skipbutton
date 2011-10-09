#include <cstdio>
#include <cstdlib>
#include "msgparser.h"

CMsgParser::CMsgParser(CMpdClient& mpdclient): m_mpdclient(mpdclient)
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
    //printf("accepted %02x\n", m_prev);
    m_msg.push_back(m_prev);
    ProcessMsg();
    m_prev = -1;
  }
  else
  {
    //printf("rejected %02x\n", byte);
    m_msg.clear();
    m_prev = byte;
  }
}

void CMsgParser::ProcessMsg()
{
  /*printf("msg:");
  for (int i = 0; i < m_msg.size(); i++)
    printf(" %02x", m_msg[i]);
  printf("\n");*/

  if (m_msg[0] != (uint8_t)MSG_START || (m_msg.size() == 3 && m_msg[2] != (uint8_t)MSG_END))
  {
    //printf("cleared %i\n", __LINE__);
    m_msg.clear();
    return;
  }

  if (m_msg.size() == 3)
  {
    if (m_msg[1] == MSG_SKIP)
    {
      printf("skip\n");
      system("curl -H \"Content-type:application/json\" -d '{\"action\":\"next\"}' http://192.168.88.10:5000/control");
      //system("paplay beep.wav");
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

