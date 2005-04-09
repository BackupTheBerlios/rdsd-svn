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

/*!
  rds_open_connection() tries to establish a connection with rdsd. rds_open_connection()
  itself does not transfer any data. After a succesful rds_open_connection(), you can use
  any of the query functions.
  
  \param rdsd_path For TCP/IP, the name or IP of the server. For unix domain socket,
                   the filename the server uses.
  \param conn_type One of CONN_TYPE_TCPIP or CONN_TYPE_UNIX. The latter is only
                   possible on systems where unix domain sockets are available.
  \param port if conn_type=CONN_TYPE_TCPIP, you must pass the servers port here.
              if conn_type=CONN_TYPE_UNIX, this parameter is ignored.
  \param unix_path if conn_type=CONN_TYPE_UNIX, the name of a temporary file used to
                   bind the client socket to. This must be writeable by your user.
		   if conn_type=CONN_TYPE_TCPIP, this parameter is ignored.
  \return rds_open_connection() returns a connection handle that is needed for all
          subsequent function calls. If rds_open_connection() fails, the return value is NULL.
*/
RDSConnectionHandle rds_open_connection(char* rdsd_path, int conn_type, int port, char* unix_path)
{
  RDSconnection* conn = new RDSconnection;
  if (conn){
    if (conn->Open(rdsd_path,conn_type,port,unix_path)) return 0;
    return (void*)conn;
  } 
  return 0;
}

/*!
  rds_close_connection() closes a connection and frees the ressources used for that
  connection.
  \param hnd A valid handle returned by rds_open_connection(). After calling
             rds_close_connection(), the handle is invalid and must not be used anymore.
  \return RDS_OK on success, RDS_SOCKET_NOT_OPEN or RDS_CLOSE_ERROR on errors.
*/
int rds_close_connection(RDSConnectionHandle hnd)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  int ret = conn->Close();
  delete conn;
  return ret;
}

/*!
  rds_enum_sources() is usually the first function you will call after a successful
  rds_open_connection(). It fills a buffer with a number of ASCII lines. Each line
  represents an RDS input source that is used by rdsd. Each line starts with a number,
  followed by a colon (':'). This is the source number which you need to query data
  from that source. The rest of the line is a description of the source.
  \param hnd A valid handle returned by rds_open_connection().
  \param buf Pointer to a buffer that receives the source description strings. The buffer
             should be large enough for several strings (4 kilobytes might be a safe value).
  \param bufsize Size of the buffer pointed to by buf, in bytes.
  \return Returns RDS_OK (0) on success. If the function fails, a non-zero error code is returned.
*/
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


