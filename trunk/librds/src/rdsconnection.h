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
  //! The constructor.
  RDSconnection();
  //! The destructor.
  ~RDSconnection();
  //! Open a connection to rdsd.
  int Open(string serv_path, int conn_type, int port, string my_path);
  //! Close the connection.
  int Close();
  //! Get the list of sources known by rdsd.
  int EnumSources(char* buf, int bufsize);
  //! Set the evnt mask for a source.
  int SetEventMask(int src, rds_events_t evnt_mask);
  //! Get the active event mask for a source.
  int GetEventMask(int src, rds_events_t &evnt_mask);
  //! Get the events that occured since the last call.
  int GetEvent(int src, rds_events_t &events);
  //! Get some RDS flags from a source.
  int GetFlags(int src, rds_flags_t &flags);
  //! Get the program type code.
  int GetPTYcode(int src, int &pty_code);
  //! Get the program identification code.
  int GetPIcode(int src, int &pi_code);
  //! Get the program name.
  int GetProgramName(int src, char* buf);
  //! Get the current radio text buffer.
  int GetRadiotext(int src, char* buf);
  //! Get the last complete radio text string.
  int GetLastRadiotext(int src, char* buf);
  //! Get UTC date and time as a string.
  int GetUTCDateTimeString(int src, char* buf);
  //! Get local date and time as a string.
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
  void process_event_msg();
};

};

#endif
