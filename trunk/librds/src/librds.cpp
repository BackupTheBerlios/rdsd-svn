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


RDSConnectionHandle rds_open_connection(char* rdsd_path, int conn_type)
{
  RDSconnection* conn = new RDSconnection;
  if (conn){
    if (conn->Open(rdsd_path,conn_type)) return 0;
    return (void*)conn;
  } 
  return 0;
}

int rds_close_connection(RDSConnectionHandle hnd)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->Close();
}

int rds_set_event_mask(RDSConnectionHandle hnd, rds_events_t evnt_mask)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->SetEventMask(evnt_mask);
}

int rds_get_event_mask(RDSConnectionHandle hnd, rds_events_t &evnt_mask)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetEventMask(evnt_mask);
}

int rds_get_event(RDSConnectionHandle hnd, rds_events_t &events)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetEvent(events);
}

int rds_get_flags(RDSConnectionHandle hnd, rds_flags_t &flags)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetFlags(flags);
}

int rds_get_pty_code(RDSConnectionHandle hnd, int &pty_code)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetPTYcode(pty_code);
}

int rds_get_pi_code(RDSConnectionHandle hnd, int &pi_code)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetPIcode(pi_code);
}

int rds_get_program_name(RDSConnectionHandle hnd, char* buf)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetProgramName(buf);
}

int rds_get_radiotext(RDSConnectionHandle hnd, char* buf)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetRadiotext(buf);
}

int rds_get_last_radiotext(RDSConnectionHandle hnd, char* buf)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetLastRadiotext(buf);
}

int rds_get_utc_datetime_string(RDSConnectionHandle hnd, char* buf)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetUTCDateTimeString(buf);
}

int rds_get_local_datetime_string(RDSConnectionHandle hnd, char* buf)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetLocalDateTimeString(buf);
}


