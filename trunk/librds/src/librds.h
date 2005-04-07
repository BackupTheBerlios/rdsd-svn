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

#ifndef LIBRDS_H
#define LIBRDS_H

typedef void* RDSConnectionHandle;

const int CONN_TYPE_TCPIP = 1;
const int CONN_TYPE_UNIX  = 2;

enum LibRdsErr {
  RDS_OK = 0,
  RDS_UNKNOWN_ERROR,
  RDS_ILLEGAL_CONN_TYPE,
  RDS_SERVER_NOT_FOUND,
  RDS_SOCKET_ERROR,
  RDS_CONNECT_ERROR,
  RDS_OPEN_ERROR,
  RDS_SOCKET_NOT_OPEN,
  RDS_CLOSE_ERROR,
  RDS_SOCKET_ALREADY_OPEN,
  RDS_BIND_ERROR,
  RDS_CHMOD_ERROR,
  RDS_WRITE_ERROR,
  RDS_READ_ERROR,
}; 

typedef unsigned long rds_flags_t;

const rds_flags_t RDS_FLAG_IS_TP              = 0x0001;
const rds_flags_t RDS_FLAG_IS_TA              = 0x0002;
const rds_flags_t RDS_FLAG_IS_MUSIC           = 0x0004;
const rds_flags_t RDS_FLAG_IS_STEREO          = 0x0008;
const rds_flags_t RDS_FLAG_IS_ARTIFICIAL_HEAD = 0x0010;
const rds_flags_t RDS_FLAG_IS_COMPRESSED      = 0x0020;
const rds_flags_t RDS_FLAG_IS_DYNAMIC_PTY     = 0x0040;
const rds_flags_t RDS_FLAG_TEXT_AB            = 0x0080;

typedef unsigned long rds_events_t;

const rds_events_t RDS_EVENT_FLAGS          = 0x0001;
const rds_events_t RDS_EVENT_PI_CODE        = 0x0002;
const rds_events_t RDS_EVENT_PTY_CODE       = 0x0004;
const rds_events_t RDS_EVENT_PROGRAMNAME    = 0x0008;
const rds_events_t RDS_EVENT_DATETIME       = 0x0010;
const rds_events_t RDS_EVENT_RADIOTEXT      = 0x0020;
const rds_events_t RDS_EVENT_LAST_RADIOTEXT = 0x0040;

extern "C" {

RDSConnectionHandle rds_open_connection(char* rdsd_path, int conn_type, int port, char* unix_path);
int rds_close_connection(RDSConnectionHandle hnd);
int rds_enum_sources(RDSConnectionHandle hnd, char* buf);
int rds_set_event_mask(RDSConnectionHandle hnd, int src, rds_events_t evnt_mask);
int rds_get_event_mask(RDSConnectionHandle hnd, int src, rds_events_t &evnt_mask);
int rds_get_event(RDSConnectionHandle hnd, int src, rds_events_t &events);
int rds_get_flags(RDSConnectionHandle hnd, int src, rds_flags_t &flags);
int rds_get_pty_code(RDSConnectionHandle hnd, int src, int &pty_code);
int rds_get_pi_code(RDSConnectionHandle hnd, int src, int &pi_code);
int rds_get_program_name(RDSConnectionHandle hnd, int src, char* buf);
int rds_get_radiotext(RDSConnectionHandle hnd, int src, char* buf);
int rds_get_last_radiotext(RDSConnectionHandle hnd, int src, char* buf);
int rds_get_utc_datetime_string(RDSConnectionHandle hnd, int src, char* buf);
int rds_get_local_datetime_string(RDSConnectionHandle hnd, int src, char* buf);

}

#endif
