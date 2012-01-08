#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <string>
#include "util/serialport.h"
#include "util/misc.h"
#include "msgparser.h"
#include "mpdclient.h"
#include "curlclient.h"

using namespace std;

void PrintHelp();

int main (int argc, char *argv[])
{
  int c;
  const char* serialport  = NULL;
  const char* mpdaddress  = "127.0.0.1";
  int         mpdport     = 6600;
  const char* fookeurl    = "http://127.0.0.1:5000/control";
  bool        bfork       = false;
  int         skiptimeout = 250000;
  while ((c = getopt(argc, argv, "hm:p:s:u:ft:")) != -1)
  {
    if (c == 'h')
    {
      PrintHelp();
      return 0;
    }
    else if (c == 'm')
    {
      mpdaddress = optarg;
    }
    else if (c == 'p')
    {
      if (!StrToInt(optarg, mpdport))
      {
        printf("Error: invalid mpd port \"%s\"\n\n", optarg);
        PrintHelp();
        return 1;
      }
    }
    else if (c == 's')
    {
      serialport = optarg;
    }
    else if (c == 'u')
    {
      fookeurl = optarg;
    }
    else if (c == 'f')
    {
      bfork = true;
    }
    else if (c == 't')
    {
      double fskiptimeout;
      if (sscanf(optarg, "%lf", &fskiptimeout) != 1)
      {
        printf("Error: invalid skip timeout \"%s\"\n\n", optarg);
        PrintHelp();
        return 1;
      }

      skiptimeout = fskiptimeout * 1000000.0 + 0.5;
    }
    else if (c == '?')
    {
      printf("\n");
      PrintHelp();
      return 1;
    }
  }

  if (bfork && fork() != 0)
    return 0;

  if (serialport == NULL)
  {
    printf("Error: no serial port given\n\n");
    PrintHelp();
    return 1;
  }

  CSerialPort port;
  if (!port.Open(serialport, 600))
  {
    printf("Failed to open port: %s\n", port.GetError().c_str());
    return 1;
  }

  CMpdClient mpdclient(mpdaddress, mpdport);
  mpdclient.StartThread();

  CCurlClient curlclient(fookeurl);
  curlclient.StartThread();

  CMsgParser parser(mpdclient, curlclient, skiptimeout);

  uint8_t in[1000];
  int size;
  while(1)
  {
    if ((size = port.Read(in, sizeof(in), 10000000)) > 0)
    {
      parser.AddData(in, size);
    }
    else if (size < 0)
    {
      printf("Error reading port: %s\n", port.GetError().c_str());
      break;
    }
  }

  curlclient.Stop();

  return 0;
}

void PrintHelp()
{
  printf(
  "Usage: skipbutton [option]\n"
  "\n"
  "  -s <serialport>     device node of the serial port (mandatory)\n"
  "  -m <address>        address of the mpd server (optional, default is 127.0.0.1)\n"
  "  -p <port>           port of the mpd server (optional, default is 6600)\n"
  "  -u <fookebox url>   url of fookebox control\n"
  "                      (optional, default is http://127.0.0.1:5000/control)\n"
  "  -f                  fork\n"
  "  -t                  skip timeout in seconds, default is 0.25\n"
  "  -h                  print this message\n"
  "\n"
  );
}

