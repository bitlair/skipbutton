#ifndef MPDCLIENT_H
#define MPDCLIENT_H

#include <deque>
#include <string>

#include "util/thread.h"
#include "util/condition.h"
#include "util/tcpsocket.h"

enum ECMD
{
  CMD_VOLUP,
  CMD_VOLDOWN
};

class CMpdClient : public CThread
{
  public:
    CMpdClient();
    ~CMpdClient();

    virtual void Process();
    void         ProcessCommands();
    void         VolumeUp();
    void         VolumeDown();

  private:
    bool         GetVolume(int& volume);
    bool         SetVolume(int volume);

    CTcpClientSocket m_socket;
    std::deque<ECMD> m_commands;
    CCondition       m_condition;
};


#endif //MPDCLIENT_H
