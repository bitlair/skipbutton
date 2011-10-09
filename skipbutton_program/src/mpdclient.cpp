#include <cstdio>
#include <cstring>
#include <sstream>

#include "mpdclient.h"
#include "util/timeutils.h"
#include "util/lock.h"
#include "util/misc.h"

using namespace std;

CMpdClient::CMpdClient()
{
}

CMpdClient::~CMpdClient()
{
}

void CMpdClient::Process()
{
  while(!m_stop)
  {
    const char* address = "192.168.88.10";
    int port = 6600;
    int returnv = m_socket.Open(address, port, 10000000);
    if (returnv != SUCCESS)
    {
      printf("Error connecting to %s:%i %s\n", address, port, m_socket.GetError().c_str());
      if (returnv != TIMEOUT)
        USleep(10000000, &m_stop);
    }
    else
    {
      printf("Connected to %s:%i\n", address, port);
      ProcessCommands();
    }

    CLock lock(m_condition);
    m_commands.clear();
  }
}

void CMpdClient::ProcessCommands()
{
  while (!m_stop)
  {
    CLock lock(m_condition);
    m_condition.Wait();
    while (!m_commands.empty())
    {
      ECMD cmd = m_commands.front();
      m_commands.pop_front();
      lock.Leave();

      int volume;
      if (!GetVolume(volume))
        return;

      if (cmd == CMD_VOLUP)
        volume += 5;
      else if (cmd == CMD_VOLDOWN)
        volume -= 5;

      if (!SetVolume(Clamp(volume, 0, 100)))
        return;

      lock.Enter();
    }
  }
}

bool CMpdClient::GetVolume(int& volume)
{
  CTcpData data;
  data.SetData("status\n");
  if (m_socket.Write(data) != SUCCESS)
  {
    printf("Error writing socket: %s\n", m_socket.GetError().c_str());
    return false;
  }

  data.Clear();
  while(1)
  {
    if (m_socket.Read(data) != SUCCESS)
    {
      printf("Error reading socket: %s\n", m_socket.GetError().c_str());
      return false;
    }

    printf("%s\n", data.GetData());

    stringstream datastream(data.GetData());
    string line;
    while (1)
    {
      getline(datastream, line);
      if (datastream.fail())
        break;

      string word;
      if (GetWord(line, word) && word == "volume:")
      {
        if (GetWord(line, word) && StrToInt(word, volume) && volume >= 0 && volume <= 100)
          return true;
      }
    }
  }
}

bool CMpdClient::SetVolume(int volume)
{
  CTcpData data;
  data.SetData(string("setvol ") + ToString(volume) + "\n");

  if (m_socket.Write(data) != SUCCESS)
  {
    printf("Error writing socket: %s\n", m_socket.GetError().c_str());
    return false;
  }
}

void CMpdClient::VolumeUp()
{
  CLock lock(m_condition);
  m_commands.push_back(CMD_VOLUP);
  m_condition.Signal();
}

void CMpdClient::VolumeDown()
{
  CLock lock(m_condition);
  m_commands.push_back(CMD_VOLDOWN);
  m_condition.Signal();
}


