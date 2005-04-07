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
#include <sys/stat.h>
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
  read_buf.resize(2048);
  last_scan_state = 0;
}


RDSconnection::~RDSconnection()
{
  Close();
}

int RDSconnection::Open(string serv_path, int conn_type, int port, string my_path)
{
  switch (conn_type){
    case CONN_TYPE_TCPIP: return open_tcpip(serv_path,port); 
                          break;
    case CONN_TYPE_UNIX:  return open_unix(serv_path,my_path);
                          break;
    default: return RDS_ILLEGAL_CONN_TYPE;
  }
}

int RDSconnection::Close()
{
  if (sock_fd < 0) return RDS_SOCKET_NOT_OPEN;
  if (close(sock_fd)) return RDS_CLOSE_ERROR;
  return RDS_OK;
}

int RDSconnection::EnumSources(char* buf, int bufsize)
{

  return 0;
}

int RDSconnection::SetEventMask(int src, rds_events_t evnt_mask)
{
  ostringstream oss;
  oss << "sevnt " << evnt_mask; 
  int ret = send_command(src,oss.str());
  if (ret) return ret;

  //Wait for acknowledge...
  
  return RDS_OK;
}

int RDSconnection::GetEventMask(int src, rds_events_t &evnt_mask)
{
  int ret = send_command(src,"gevnt");
  if (ret) return ret;

  //Wait for response...
  
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
  if (sock_fd>=0) return RDS_SOCKET_ALREADY_OPEN;
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

int RDSconnection::open_unix(string serv_path, string my_path)
{
  if (sock_fd>=0) return RDS_SOCKET_ALREADY_OPEN;
  
  struct sockaddr_un sock_addr;
  // Create unix domain socket
  sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock_fd < 0) return RDS_SOCKET_ERROR;
  // Write sock_addr with own address
  memset(&sock_addr,0,sizeof(sock_addr));
  sock_addr.sun_family = AF_UNIX;
  strncpy(sock_addr.sun_path,my_path.c_str(),sizeof(sock_addr.sun_path)-1);
  int size = strlen(sock_addr.sun_path)+sizeof(sock_addr.sun_family);
  unlink(sock_addr.sun_path);
  if (bind(sock_fd,(struct sockaddr*)&sock_addr,size)<0){
    close(sock_fd);
    return RDS_BIND_ERROR;
  }
  if (chmod(sock_addr.sun_path,S_IRWXU)<0){
    Close();
    return RDS_CHMOD_ERROR;
  }
  // Write sock_addr with server address
  memset(&sock_addr,0,sizeof(sock_addr));
  sock_addr.sun_family = AF_UNIX;
  strncpy(sock_addr.sun_path,serv_path.c_str(),sizeof(sock_addr.sun_path)-1);
  size = strlen(sock_addr.sun_path)+sizeof(sock_addr.sun_family);

  if (connect(sock_fd,(struct sockaddr*)&sock_addr,size)<0){
    Close();
    return RDS_CONNECT_ERROR;
  }
  return RDS_OK;
}

int RDSconnection::send_command(int src, string cmd)
{
  if (sock_fd<0) return RDS_SOCKET_NOT_OPEN;
  ostringstream oss;
  if (src>=0) oss << src << ":";
  oss << cmd << endl;
  int n = oss.str().size();
  if (write(sock_fd,oss.str().c_str(),n)!=n) return RDS_WRITE_ERROR;
  return RDS_OK;
}

int RDSconnection::process()
{
  if (sock_fd<0) return RDS_SOCKET_NOT_OPEN;
  int rd_cnt = read(sock_fd,&read_buf[0],read_buf.size());
  if (rd_cnt<0) return RDS_READ_ERROR;
  enum ScanState {ssEOL=0,ssData,ssComment,ssTerm};
  ScanState state = (ScanState)last_scan_state;
  int i=0;
  while (i<rd_cnt){
    char ch = read_buf[i];
    switch (ch){
      case '\n':
      case '\r': if (state == ssTerm) process_msg();
                 else read_str.push_back(ch);
                 state = ssEOL;
                 break;
      case '.' : if (state == ssEOL) state=ssTerm;
                 else read_str.push_back(ch);
                 break;
      case '#' : if (state == ssEOL) state=ssComment;
                 else read_str.push_back(ch);
		 break;
      default:   switch (state){
                   case ssEOL:  read_str.push_back(ch);
		                state = ssData;
			        break;
	           case ssData: read_str.push_back(ch);
		                break;
		   case ssTerm: read_str.push_back('.');
		                read_str.push_back(ch);
		                state = ssData;
			        break;
		   case ssComment: ;
                 }
    }
    last_scan_state = state;
    ++i;
  }
  return RDS_OK;
}

void RDSconnection::process_msg()
{

}

};
