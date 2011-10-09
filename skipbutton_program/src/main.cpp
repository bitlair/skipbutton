#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <string>
#include "util/serialport.h"
#include "util/misc.h"
#include "msgparser.h"
#include "mpdclient.h"

using namespace std;

int main (int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("ERROR: first argument needs to be the serial port\n");
    return 1;
  }

  CSerialPort port;
  if (!port.Open(argv[1], 600))
  {
    printf("Failed to open port: %s\n", port.GetError().c_str());
    return 1;
  }

  uint8_t in[1000];
  int size;
  CMpdClient mpdclient;
  mpdclient.StartThread();
  CMsgParser parser(mpdclient);
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

  return 0;
}

