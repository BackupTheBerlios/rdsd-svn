/*****************************************************************************
 *   This file is part of librds, a library to fetch data from the           *
 *   Radio Data System Daemon (rdsd). See http://rdsd.berlios.de             * 
 *   Copyright (C) 2005 by Hans J. Koch                                      *
 *   hjkoch@users.berlios.de                                                 *
 *                                                                           *
 *   This library is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU Lesser General Public              *
 *   License as published by the Free Software Foundation; either            *
 *   version 2.1 of the License, or (at your option) any later version.      *
 *                                                                           * 
 *   This library is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 *   Lesser General Public License for more details.                         *
 *                                                                           *
 *   You should have received a copy of the GNU Lesser General Public        *    
 *   License along with this library; if not, write to the Free Software     *
 *   Foundation,Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA *
 *****************************************************************************/


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
  Create a connection object. The handle returned by this function is used for
  all subsequent calls to other functions.
  \return A handle used to refer to this connection object.
*/
RDSConnectionHandle rds_create_connection_object()
{
  RDSconnection* conn = new RDSconnection;
  return conn;
}
/*!
  Delete the connection object and free the associated resources. After a call
  rds_delete_connection_object(), the handle is invalid and must not be used again.
  \param hnd A valid handle returned by rds_create_connection_object().
  \return At the moment, rds_delete_connection_object() always returns zero.
*/
int rds_delete_connection_object(RDSConnectionHandle hnd)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  delete conn;
  return 0;
}

/*!
  rds_open_connection() tries to establish a connection with rdsd. rds_open_connection()
  itself does not transfer any data. After a succesful rds_open_connection(), you can use
  any of the query functions.

  \param hnd A valid handle returned by rds_create_connection_object().
  \param rdsd_path For TCP/IP, the name or IP of the server. For unix domain socket,
                   the filename the server uses.
  \param conn_type One of CONN_TYPE_TCPIP or CONN_TYPE_UNIX. The latter is only
                   possible on systems where unix domain sockets are available.
  \param port if conn_type=CONN_TYPE_TCPIP, you must pass the servers port here.
              if conn_type=CONN_TYPE_UNIX, this parameter is ignored.
  \param unix_path if conn_type=CONN_TYPE_UNIX, the name of a temporary file used to
                   bind the client socket to. This must be writeable by your user.
		   if conn_type=CONN_TYPE_TCPIP, this parameter is ignored.
  \return rds_open_connection() returns RDS_OK on success or a non-zero error value.
*/
int rds_open_connection(RDSConnectionHandle hnd, const char* rdsd_path, int conn_type,
                        int port, const char* unix_path)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->Open(rdsd_path,conn_type,port,unix_path);
}

/*!
  rds_close_connection() closes a connection. It can later be opened again using
  rds_open_connection().

  \param hnd A valid handle returned by rds_create_connection_object(). After calling
             rds_close_connection(), subsequent calls to query functions will fail.
  \return RDS_OK on success, RDS_SOCKET_NOT_OPEN or RDS_CLOSE_ERROR on errors.
*/
int rds_close_connection(RDSConnectionHandle hnd)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  int ret = conn->Close();
  return ret;
}

/*!
  Set the timeout for the communication with rdsd.
  \param hnd A valid handle returned by rds_create_connection_object().
  \return RDS_OK on success, RDS_ILLEGAL_TIMEOUT if the time is too big or too small.
*/
int rds_set_timeout_time(RDSConnectionHandle hnd, unsigned int timeout_msec)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->SetTimeoutTime(timeout_msec);
}

/*!
  Set debug parameters.
  \param hnd A valid handle returned by rds_create_connection_object().
  \param debug_level The higher this value, the more information you get. 0 turns debugging off.
  \param max_lines Maximum number of lines stored in the internal ring buffer.
  \return RDS_OK on success.
*/
int rds_set_debug_params(RDSConnectionHandle hnd, int debug_level, unsigned int max_lines)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->SetDebugParams(debug_level,max_lines);
}

/*!
  Get stored debug messages or query the required buffer size.
  \param hnd A valid handle returned by rds_create_connection_object().
  \param buf      Pointer to a buffer to receive the text. The user is responsible
                  for the allocation of this buffer.
  \param buf_size A variable that contains the size in chars of the buffer pointed
                  to by buf. If buf is a NULL pointer or buf_size has a zero value,
		  buf_size will receive the required size for buf.
  \return RDS_OK on success.
*/
int rds_get_debug_text(RDSConnectionHandle hnd, char* buf, unsigned int& buf_size)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetDebugTextBuffer(buf,buf_size);
}

/*!
  rds_enum_sources() is usually the first function you will call after a successful
  rds_open_connection(). It fills a buffer with a number of ASCII lines. Each line
  represents an RDS input source that is used by rdsd. Each line starts with a number,
  followed by a colon (':'). This is the source number which you need to query data
  from that source. The rest of the line is a description of the source.
  The lines are separated by LF characters. The whole string is zero-terminated.
  \param hnd A valid handle returned by rds_create_connection_object().
  \param buf Pointer to a buffer that receives the source description strings. The buffer
             should be large enough for several strings (4 kilobytes might be a safe value).
  \param bufsize Size of the buffer pointed to by buf, in bytes.
  \return Returns RDS_OK (0) on success. If the function fails, a non-zero error code is returned.
*/
int rds_enum_sources(RDSConnectionHandle hnd, char* buf, size_t bufsize)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->EnumSources(buf,bufsize);
}

/*!
  Set the events you are waiting for. You will only receive events from rdsd that
  have their corresponding bit set in evnt_mask.
  \param hnd A valid handle returned by rds_create_connection_object().
  \param src A valid source number, one of those returned by rds_enum_sources().
  \param evnt_mask The event mask to be set. This is a combination of one or more
             of the RDS_EVENT_* constants defined in librds.h
  \return RDS_OK on success,
*/
int rds_set_event_mask(RDSConnectionHandle hnd, int src, rds_events_t evnt_mask)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->SetEventMask(src, evnt_mask);
}

/*!
  Get the event mask that was previously set with rds_set_event_mask().
  \param hnd A valid handle returned by rds_create_connection_object().
  \param src A valid source number, one of those returned by rds_enum_sources().
  \param evnt_mask A variable that receives the event mask.
  \return RDS_OK on success,
*/
int rds_get_event_mask(RDSConnectionHandle hnd, int src, rds_events_t &evnt_mask)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetEventMask(src, evnt_mask);
}

/*!
  
  \param hnd A valid handle returned by rds_create_connection_object().
  \param src A valid source number, one of those returned by rds_enum_sources().
  \return RDS_OK on success,
*/
int rds_get_event(RDSConnectionHandle hnd, int src, rds_events_t &events)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetEvent(src, events);
}

/*!

  \param hnd A valid handle returned by rds_create_connection_object().
  \param src A valid source number, one of those returned by rds_enum_sources().
  \return RDS_OK on success,
*/
int rds_get_flags(RDSConnectionHandle hnd, int src, rds_flags_t &flags)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetFlags(src, flags);
}

/*!

  \param hnd A valid handle returned by rds_create_connection_object().
  \param src A valid source number, one of those returned by rds_enum_sources().
  \return RDS_OK on success,
*/
int rds_get_pty_code(RDSConnectionHandle hnd, int src, int &pty_code)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetPTYcode(src, pty_code);
}

/*!

  \param hnd A valid handle returned by rds_create_connection_object().
  \param src A valid source number, one of those returned by rds_enum_sources().
  \return RDS_OK on success,
*/
int rds_get_pi_code(RDSConnectionHandle hnd, int src, int &pi_code)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetPIcode(src, pi_code);
}

/*!

  \param hnd A valid handle returned by rds_create_connection_object().
  \param src A valid source number, one of those returned by rds_enum_sources().
  \return RDS_OK on success,
*/
int rds_get_program_name(RDSConnectionHandle hnd, int src, char* buf)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetProgramName(src, buf);
}

/*!

  \param hnd A valid handle returned by rds_create_connection_object().
  \param src A valid source number, one of those returned by rds_enum_sources().
  \return RDS_OK on success,
*/
int rds_get_radiotext(RDSConnectionHandle hnd, int src, char* buf)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetRadiotext(src, buf);
}

/*!

  \param hnd A valid handle returned by rds_create_connection_object().
  \param src A valid source number, one of those returned by rds_enum_sources().
  \return RDS_OK on success,
*/
int rds_get_last_radiotext(RDSConnectionHandle hnd, int src, char* buf)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetLastRadiotext(src, buf);
}

/*!

  \param hnd A valid handle returned by rds_create_connection_object().
  \param src A valid source number, one of those returned by rds_enum_sources().
  \return RDS_OK on success,
*/
int rds_get_utc_datetime_string(RDSConnectionHandle hnd, int src, char* buf)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetUTCDateTimeString(src, buf);
}

/*!

  \param hnd A valid handle returned by rds_create_connection_object().
  \param src A valid source number, one of those returned by rds_enum_sources().
  \return RDS_OK on success,
*/
int rds_get_local_datetime_string(RDSConnectionHandle hnd, int src, char* buf)
{
  RDSconnection* conn = (RDSconnection*)hnd;
  return conn->GetLocalDateTimeString(src, buf);
}

/*!
  Get the TMC message buffer or query the required buffer size.
*/
int rds_get_tmc_buffer(RDSConnectionHandle hnd, int src, char* buf, size_t &bufsize)
{
  return RDS_NOT_IMPLEMENTED;
}
