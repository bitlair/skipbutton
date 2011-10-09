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
  CLock lock(m_condition);

  const char* address = "192.168.88.10";
  int port = 6600;

  while(!m_stop)
  {
    if (m_socket.IsOpen())
      m_condition.Wait(10000000);
    else
      m_condition.Wait();

    if (!m_commands.empty())
    {
      if (!m_socket.IsOpen())
      {
        int returnv = m_socket.Open(address, port, 10000000);
        if (returnv != SUCCESS)
        {
          lock.Leave();
          printf("Error connecting to %s:%i %s\n", address, port, m_socket.GetError().c_str());
          if (returnv != TIMEOUT)
            USleep(10000000, &m_stop);

          m_socket.Close();

          lock.Enter();
          m_commands.clear();
          continue;
        }
        else
        {
          printf("Connected to %s:%i\n", address, port);
        }
      }

      while (!m_commands.empty())
      {
        ECMD cmd = m_commands.front();
        m_commands.pop_front();
        lock.Leave();

        int volume;
        if (!GetVolume(volume))
          break;

        if (cmd == CMD_VOLUP)
          volume += 5;
        else if (cmd == CMD_VOLDOWN)
          volume -= 5;

        printf("Setting volume to %i\n", volume);

        if (!SetVolume(Clamp(volume, 0, 100)))
          break;

        lock.Enter();
      }

      lock.Enter();
      if (!m_commands.empty())
      {
        m_commands.clear();
        m_socket.Close();
      }
    }
    else
    {
      printf("Closing connection to %s:%i\n", address, port);
      m_socket.Close();
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

