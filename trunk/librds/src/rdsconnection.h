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

#include <string>
#include "librds.h"

namespace std {

/**
Class used in librds. For each client connection to rdsd, one object of this class will be created. A pointer to this object will be returned to the application as a handle to use this connection.

@author Hans J. Koch
*/
class RDSconnection{
public:
  RDSconnection();
  ~RDSconnection();
  int Open(string path, int conn_type);
  int Close();
  int SetEventMask(rds_events_t evnt_mask);
  int GetEventMask(rds_events_t &evnt_mask);
  int GetEvent(rds_events_t &events);
  int GetFlags(rds_flags_t &flags);
  int GetPTYcode(int &pty_code);
  int GetPIcode(int &pi_code);

  int GetProgramName(char* buf);
  int GetRadiotext(char* buf);
  int GetLastRadiotext(char* buf);
  int GetUTCDateTimeString(char* buf);
  int GetLocalDateTimeString(char* buf);
};

};

#endif
