/*
 * boblight
 * Copyright (C) Bob  2009 
 * 
 * boblight is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * boblight is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CSERIALPORT
#define CSERIALPORT

#define __STDC_CONSTANT_MACROS
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <termios.h>

#include <string>
#include <vector>

#define PAR_NONE 0
#define PAR_EVEN 1
#define PAR_ODD  2

class CSerialPort
{
  public:
    CSerialPort();
    ~CSerialPort();

    bool Open(std::string name, int baudrate, int databits = 8, int stopbits = 1, int parity = PAR_NONE);
    void Close();
    int  Write(std::vector<uint8_t> data);
    int  Write(uint8_t* data, int len);
    int  Read(uint8_t* data, int len, int64_t usecs = -1);
    int  IntToRate(int baudrate);
    bool IsOpen() { return m_fd != -1; }
    void SetDTR(bool high);
    void SetRTS(bool high);
    void PrintToStdOut(bool tostdout) { m_tostdout = tostdout; }

    std::string GetError();
    std::string GetName();

  private:
    int         m_fd;
    std::string m_error;
    std::string m_name;
    bool        m_tostdout;

    struct termios m_options;

    bool SetBaudRate(int baudrate);
};

#endif //CSERIALPORT
