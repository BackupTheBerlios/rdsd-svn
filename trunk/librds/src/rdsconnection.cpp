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

/*!
  The default constructor just does some variable initialization.
*/
RDSconnection::RDSconnection()
{
  sock_fd = -1;
  read_buf.resize(2048);
  last_scan_state = 0;
}

/*!
  The destructor also closes the socket if it is still open.
*/
RDSconnection::~RDSconnection()
{
  Close();
}

/*!
  Open() tries to establish a connection with rdsd. Open() itself does not
  transfer any data. After a succesful Open(), you can use any of the query
  functions. You can also use SetEventMask() to receive notifications if
  an RDS source has decoded new data.
  \param serv_path For TCP/IP, the name or IP of the server. For unix domain socket,
                   the filename the server uses.
  \param conn_type One of CONN_TYPE_TCPIP or CONN_TYPE_UNIX. The latter is only
                   possible on systems where unix domain sockets are available.
  \param port if conn_type=CONN_TYPE_TCPIP, you must pass the servers port here.
              if conn_type=CONN_TYPE_UNIX, this parameter is ignored.
  \param my_path if conn_type=CONN_TYPE_UNIX, the name of a temporary file used to
                 bind the client socket to. This must be writeable by your user.
  \return On success, Open() returns RDS_OK (0). Otherwise, a positive error code is
          returned.
*/
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

/*!
  Close() closes a connection. Only one connection can be handled by a RDSconnection
  object. You need to call Close() before you can open a new connection.
  \return RDS_OK on success, RDS_SOCKET_NOT_OPEN or RDS_CLOSE_ERROR on errors.
*/
int RDSconnection::Close()
{
  if (sock_fd < 0) return RDS_SOCKET_NOT_OPEN;
  if (close(sock_fd)) return RDS_CLOSE_ERROR;
  return RDS_OK;
}

/*!
  EnumSources() is usually the first function you will call after a successful Open().
  It fills a buffer with a number of ASCII lines. Each line represents an RDS input
  source that is used by rdsd. Each line starts with a number, followed by a colon (':').
  This is the source number which you need to query data from that source. The rest of
  the line is a description of the source.
  \param buf Pointer to a buffer that receives the source description strings. The buffer
             should be large enough for several strings (4 kilobytes might be a safe value).
  \param bufsize Size of the buffer pointed to by buf, in bytes.
  \return Returns RDS_OK (0) on success. If the function fails, a non-zero error code is returned.
*/
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
  enum ScanState {ssEOL=0,ssData,ssComment,ssTerm,ssEvent};
  ScanState state = (ScanState)last_scan_state;
  int i=0;
  while (i<rd_cnt){
    char ch = read_buf[i];
    switch (ch){
      case '\n':
      case '\r': switch (state) {
                   case ssTerm:  process_msg();
		                 break;
		   case ssEvent: process_event_msg();
		                 break; 
		   case ssEOL:   break;
                   default: read_str.push_back(ch);
		 }
		 state = ssEOL;
                 break;
      case '!' : if (state == ssEOL) state=ssEvent;
                 else read_str.push_back(ch);
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
		   case ssEvent:
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

  read_str = "";
}

void RDSconnection::process_event_msg()
{
  string num_str;
  string evnt_str;
  bool is_num_str = true;
  for (unsigned int i=0; i<read_str.size(); i++){
    char ch = read_str[i];
    if (ch == ':') is_num_str = false;
    else {
      if (is_num_str) num_str += ch; else evnt_str += ch;
    }
  }
  
}

};
