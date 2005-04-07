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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include "librds.h"
#include "rdsconnection.h"
#include "rdsconnection.h"

using namespace std;


RDSConnectionHandle rds_open_connection(char* rdsd_path, int conn_type, int port, char* unix_path)
{
  RDSconnection* conn = new RDSconnection;
  if (conn){
    if (conn->Open(rdsd_path,conn_type,port,unix_path)) return 0;
    return (void*)conn;
  } 
  return 0;
}

int rds_close_connection(RDSConnectionHandle hnd)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  int ret = conn->Close();
  delete conn;
  return ret;
}

int rds_enum_sources(RDSConnectionHandle hnd, char* buf, int bufsize)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->EnumSources(buf,bufsize);
}

int rds_set_event_mask(RDSConnectionHandle hnd, int src, rds_events_t evnt_mask)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->SetEventMask(src, evnt_mask);
}

int rds_get_event_mask(RDSConnectionHandle hnd, int src, rds_events_t &evnt_mask)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetEventMask(src, evnt_mask);
}

int rds_get_event(RDSConnectionHandle hnd, int src, rds_events_t &events)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetEvent(src, events);
}

int rds_get_flags(RDSConnectionHandle hnd, int src, rds_flags_t &flags)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetFlags(src, flags);
}

int rds_get_pty_code(RDSConnectionHandle hnd, int src, int &pty_code)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetPTYcode(src, pty_code);
}

int rds_get_pi_code(RDSConnectionHandle hnd, int src, int &pi_code)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetPIcode(src, pi_code);
}

int rds_get_program_name(RDSConnectionHandle hnd, int src, char* buf)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetProgramName(src, buf);
}

int rds_get_radiotext(RDSConnectionHandle hnd, int src, char* buf)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetRadiotext(src, buf);
}

int rds_get_last_radiotext(RDSConnectionHandle hnd, int src, char* buf)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetLastRadiotext(src, buf);
}

int rds_get_utc_datetime_string(RDSConnectionHandle hnd, int src, char* buf)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetUTCDateTimeString(src, buf);
}

int rds_get_local_datetime_string(RDSConnectionHandle hnd, int src, char* buf)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetLocalDateTimeString(src, buf);
}


