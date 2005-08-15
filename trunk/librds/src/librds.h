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

//! Type of the handle returned by rds_open_connection()
/*!
  Presently, a pointer to an object of class RDSconnection is used as a handle.
  Applications using librds should not rely on this. The typedef for
  RDSConnectionHandle might change in the future.
*/
typedef void* RDSConnectionHandle;

const unsigned int MAX_SRC_NUM = 255;   //!< Maximum RDS source number

const int CONN_TYPE_TCPIP = 1; //!< Connection uses TCP/IP
const int CONN_TYPE_UNIX  = 2; //!< Connection uses unix domain socket

//! Error codes returned by librds API functions
/*!
  librds API functions return one of these values (cast to int). It is guaranteed that
  RDS_OK (returned on success) will always be zero. The other values are positive integers. 
*/
enum LibRdsErr {
  RDS_OK = 0,                //!< Success, no errors
  RDS_UNKNOWN_ERROR,         //!< Error, reason is not clear
  RDS_ILLEGAL_CONN_TYPE,     //!< Connection type is neither CONN_TYPE_TCPIP nor CONN_TYPE_UNIX 
  RDS_SERVER_NOT_FOUND,      //!< The rdsd server could not be found
  RDS_SOCKET_ERROR,          //!< The socket() system call failed
  RDS_CONNECT_ERROR,         //!< The connect() system call failed
  RDS_OPEN_ERROR,            //!< The open() system call failed  
  RDS_SOCKET_NOT_OPEN,       //!< There is no valid socket file descriptor for a connection
  RDS_CLOSE_ERROR,           //!< The close() system call failed
  RDS_SOCKET_ALREADY_OPEN,   //!< Attempt to open a connection while there is still an open socket 
  RDS_BIND_ERROR,            //!< The bind() system call failed
  RDS_CHMOD_ERROR,           //!< The chmod() system call failed
  RDS_WRITE_ERROR,           //!< The write() system call failed
  RDS_READ_ERROR,            //!< The read() system call failed
  RDS_ILL_SRC_NUM            //!< An illegal source number was given 
}; 

//! Type for a variable that stores RDS flags.
/*!
  RDS data contains a number of one-bit flags. To save space, time, and network traffic,
  these flags are stored together in one variable of type rds_flags_t.
*/
typedef unsigned long rds_flags_t;

const rds_flags_t RDS_FLAG_IS_TP              = 0x0001; //!< Program is a traffic program
const rds_flags_t RDS_FLAG_IS_TA              = 0x0002; //!< Program currently broadcasts a traffic announcement
const rds_flags_t RDS_FLAG_IS_MUSIC           = 0x0004; //!< Program currently broadcasts music
const rds_flags_t RDS_FLAG_IS_STEREO          = 0x0008; //!< Program is transmitted in stereo
const rds_flags_t RDS_FLAG_IS_ARTIFICIAL_HEAD = 0x0010; //!< Program is an artificial head recording
const rds_flags_t RDS_FLAG_IS_COMPRESSED      = 0x0020; //!< Program content is compressed
const rds_flags_t RDS_FLAG_IS_DYNAMIC_PTY     = 0x0040; //!< Program type can change 
const rds_flags_t RDS_FLAG_TEXT_AB            = 0x0080; //!< If this flag changes state, a new radio text string begins

//! Type for a variable that stores RDS events.
/*!
  RDS events are stored in a variable where each event is represented
  by one bit. If an event occured, the respective bit is set to 1.
*/
typedef unsigned long rds_events_t;

const rds_events_t RDS_EVENT_FLAGS          = 0x0001; //!< One of the RDS flags has changed state
const rds_events_t RDS_EVENT_PI_CODE        = 0x0002; //!< The program identification code has changed
const rds_events_t RDS_EVENT_PTY_CODE       = 0x0004; //!< The program type code has changed
const rds_events_t RDS_EVENT_PROGRAMNAME    = 0x0008; //!< The program name has changed
const rds_events_t RDS_EVENT_DATETIME       = 0x0010; //!< A new date/time info was received
const rds_events_t RDS_EVENT_RADIOTEXT      = 0x0020; //!< New characters were added to the radiotext buffer
const rds_events_t RDS_EVENT_LAST_RADIOTEXT = 0x0040; //!< A radio text string was completed

extern "C" {

//! Open a connection with rdsd.
RDSConnectionHandle rds_open_connection(char* rdsd_path, int conn_type, int port, char* unix_path);
//! Close a connection
int rds_close_connection(RDSConnectionHandle hnd);
//! Enumerate the sources that rdsd knows about.
int rds_enum_sources(RDSConnectionHandle hnd, char* buf);
//! Set the event mask for a RDS data source.
int rds_set_event_mask(RDSConnectionHandle hnd, int src, rds_events_t evnt_mask);
//! Query the event mask that is used for a RDS data source.
int rds_get_event_mask(RDSConnectionHandle hnd, int src, rds_events_t &evnt_mask);
//! Find out which events were signaled by a data source.
int rds_get_event(RDSConnectionHandle hnd, int src, rds_events_t &events);
//! Get RDS flags data.
int rds_get_flags(RDSConnectionHandle hnd, int src, rds_flags_t &flags);
//! Get RDS program type code.
int rds_get_pty_code(RDSConnectionHandle hnd, int src, int &pty_code);
//! Get RDS program identification code.
int rds_get_pi_code(RDSConnectionHandle hnd, int src, int &pi_code);
//! Get RDS program name (Usually the abbreviation of the station name).
int rds_get_program_name(RDSConnectionHandle hnd, int src, char* buf);
//! Get the current radio text buffer. 
int rds_get_radiotext(RDSConnectionHandle hnd, int src, char* buf);
//! Get the last complete radio text string.
int rds_get_last_radiotext(RDSConnectionHandle hnd, int src, char* buf);
//! Get the last UTC date/time as a string.
int rds_get_utc_datetime_string(RDSConnectionHandle hnd, int src, char* buf);
//! Get the last local date/time as a string.
int rds_get_local_datetime_string(RDSConnectionHandle hnd, int src, char* buf);

}

#endif
