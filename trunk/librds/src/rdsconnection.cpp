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
#include "rdsconnection.h"
#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
//#include <fcntl.h>
#include <sstream>

namespace std {

RDSconnection::RDSconnection()
{
  sock_fd = -1;
}


RDSconnection::~RDSconnection()
{
  Close();
}

int RDSconnection::Open(string path, int conn_type, int port)
{
  switch (conn_type){
    case CONN_TYPE_TCPIP: return open_tcpip(path,port); 
                          break;
    case CONN_TYPE_UNIX:  return open_unix(path);
                          break;
    default: return RDS_ILLEGAL_CONN_TYPE;
  }
}

int RDSconnection::Close()
{
  if (sock_fd < 0) return RDS_SOCKET_NOT_OPEN;
  if (close(sock_fd)) return RDS_CLOSE_ERROR;
  return 0;
}

int RDSconnection::EnumSources(char* buf, int bufsize)
{

  return 0;
}

int RDSconnection::SetEventMask(int src, rds_events_t evnt_mask)
{

  return 0;
}

int RDSconnection::GetEventMask(int src, rds_events_t &evnt_mask)
{

  return 0;
}

int RDSconnection::GetEvent(int src, rds_events_t &events)
{

  return 0;
}

int RDSconnection::GetFlags(int src, rds_flags_t &flags)
{

  return 0;
}

int RDSconnection::GetPTYcode(int src, int &pty_code)
{

  return 0;
}

int RDSconnection::GetPIcode(int src, int &pi_code)
{

  return 0;
}

int RDSconnection::GetProgramName(int src, char* buf)
{

  return 0;
}

int RDSconnection::GetRadiotext(int src, char* buf)
{

  return 0;
}

int RDSconnection::GetLastRadiotext(int src, char* buf)
{

  return 0;
}

int RDSconnection::GetUTCDateTimeString(int src, char* buf)
{

  return 0;
}

int RDSconnection::GetLocalDateTimeString(int src, char* buf)
{

  return 0;
}

// private member functions -------------------------------------

int RDSconnection::open_tcpip(string path, int port)
{
  struct hostent *server;
  struct in_addr inaddr;
  if (inet_aton(path.c_str(),&inaddr))
    server = gethostbyaddr((char*)&inaddr,sizeof(inaddr),AF_INET);
  else
    server = gethostbyname(path.c_str());

  if (!server) return RDS_SERVER_NOT_FOUND;

  struct sockaddr_in sock_addr;

  sock_addr.sin_family = AF_INET;
  sock_addr.sin_port = htons(port);
  memcpy(&sock_addr.sin_addr, server->h_addr_list[0], sizeof(sock_addr.sin_addr));

  sock_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) return RDS_SOCKET_ERROR;

  if (connect(sock_fd,(struct sockaddr*)&sock_addr,sizeof(sock_addr))){
    Close();
    return RDS_CONNECT_ERROR;
  }
  
  return RDS_OK;
}

int RDSconnection::open_unix(string path)
{

  return 0;
}

};
