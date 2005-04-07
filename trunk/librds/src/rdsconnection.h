/***************************************************************************
 *   Copyright (C) 2005 by Hans J. Koch                                    *
 *   hans-juergen@hjk-az.de                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef STDRDSCONNECTION_H
#define STDRDSCONNECTION_H

#include "librds.h"
#include <string>
#include <vector>

namespace std {

/**
Class used in librds. For each client connection to rdsd, one object of this class will be created. A pointer to this object will be returned to the application as a handle to use this connection.

@author Hans J. Koch
*/
class RDSconnection{
public:
  RDSconnection();
  ~RDSconnection();
  int Open(string serv_path, int conn_type, int port, string my_path);
  int Close();
  int EnumSources(char* buf, int bufsize);
  int SetEventMask(int src, rds_events_t evnt_mask);
  int GetEventMask(int src, rds_events_t &evnt_mask);
  int GetEvent(int src, rds_events_t &events);
  int GetFlags(int src, rds_flags_t &flags);
  int GetPTYcode(int src, int &pty_code);
  int GetPIcode(int src, int &pi_code);

  int GetProgramName(int src, char* buf);
  int GetRadiotext(int src, char* buf);
  int GetLastRadiotext(int src, char* buf);
  int GetUTCDateTimeString(int src, char* buf);
  int GetLocalDateTimeString(int src, char* buf);
private:
  int sock_fd;
  vector<char> read_buf;
  string read_str;
  int last_scan_state;
  int open_tcpip(string path, int port);
  int open_unix(string serv_path, string my_path);
  int send_command(int src, string cmd);
  int process();
  void process_msg();
};

};

#endif
