/***************************************************************************
 *   Copyright (C) 2005 by Hans J. Koch                                    *
 *   koch@hjk-az.de                                                *
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
#include "rdssource.h"
#include "rdsd_errors.h"
#include <fcntl.h>
#include <sstream>

namespace std {

RDSsource::RDSsource()
{
  log = 0;
  fd = -1;
  status = SRCSTAT_CLOSED;
  src_type = SRCTYPE_NONE;
}


RDSsource::~RDSsource()
{
}

string RDSsource::GetName()
{
  return srcname;
}

string RDSsource::GetPath()
{
  return srcpath;
}

void RDSsource::SetLogHandler(LogHandler *loghandler)
{
  log = loghandler;
}

string RDSsource::GetStatusStr()
{
  ostringstream result;
  result << (int)status << endl;
  switch (status){
    case SRCSTAT_OK:     result << "#OK, data available.";
                         break;
    case SRCSTAT_WAIT:   result << "#Waiting for data.";
                         break;
    case SRCSTAT_CLOSED: result << "#Source closed.";
                         break;
  }
  return result.str();
}

int RDSsource::GetFd()
{
  return fd;
}

int RDSsource::Open()
{
  Close();
  int ret;
  
  switch (src_type){
    case SRCTYPE_NONE:
           return RDSD_NO_SOURCE_TYPE;
           break;
    case SRCTYPE_RADIODEV:
    case SRCTYPE_FILE:
           ret = open(srcpath.c_str(),O_RDONLY|O_NONBLOCK);
           if (ret<0){
             LogMsg(LL_ERR,"Failed to open source: "+srcname);
             return RDSD_SOURCE_OPEN_ERROR;
           }
	   fd = ret;
           break;
    case SRCTYPE_I2CDEV:
           ret = open(srcpath.c_str(),O_RDWR); //We might need to be able to write bytes to SAA6588 registers
           if (ret<0){
             LogMsg(LL_ERR,"Failed to open source: "+srcname);
             return RDSD_SOURCE_OPEN_ERROR;
           }
	   fd = ret;
           break;
  }
  LogMsg(LL_DEBUG,"Source opened: "+srcname);
  status = SRCSTAT_WAIT;
  return RDSD_OK;
}

void RDSsource::Close()
{
  if (fd>=0) close(fd);
  fd = -1;
  status = SRCSTAT_CLOSED;
}

int RDSsource::Process()
{
  if (status != SRCSTAT_CLOSED){
    CharBuf buf(300);
    int ret;
    switch (src_type){
      case SRCTYPE_RADIODEV:
             ret = read(fd,&buf[0],buf.size());
             break;
      case SRCTYPE_FILE:
             ret = read(fd,&buf[0],3);
	     if ((ret>0)&&(ret!=3)) ret = -1;
	     usleep(10000);
             break;
      case SRCTYPE_I2CDEV:
             ret = read(fd,&buf[0],6);
	     if (ret==6){
               buf.resize(3); //use only first 3 bytes for the moment
	       ret=3;
	     }
	     else if (ret>0) ret = -1;
             break;
    }
    if (ret<0) return RDSD_SOURCE_READ_ERROR;
    if (ret>0){
      buf.resize(ret);
      Data.AddBytes(&buf);
      status = SRCSTAT_OK;
    }
    return RDSD_OK;
  }
  return 0;
}
  
int RDSsource::Init(ConfSection* sect)
{
  if (! sect) return -1;
  srcname="";
  srcpath="";
  for (int valno=0; valno < sect->GetValueCount(); ++valno){
    ConfValue* val = sect->GetValue(valno);
    if (val){
      string valname = val->GetName();
      bool valid;
      if (valname == "name"){
        srcname = val->GetString(valid);
      }
      else if (valname == "path"){
        srcpath = val->GetString(valid);
      }
      else if (valname == "type"){
        string S = val->GetString(valid);
	if (S == "radiodev") src_type = SRCTYPE_RADIODEV;
	else if (S == "i2cdev") src_type = SRCTYPE_I2CDEV;
	else if (S == "file") src_type = SRCTYPE_FILE;
	else src_type = SRCTYPE_NONE;
      }
    }
  }
  if (srcname.empty()) return RDSD_NO_SOURCE_NAME;
  if (srcpath.empty()) return RDSD_NO_SOURCE_PATH;
  if (src_type == SRCTYPE_NONE) return RDSD_NO_SOURCE_TYPE;
  LogMsg(LL_DEBUG,"Added source definition: "+srcname);
  return RDSD_OK;
}

void RDSsource::LogMsg(LogLevel prio, string msg)
{
  if (log) log->LogMsg(prio,msg);
}

};
