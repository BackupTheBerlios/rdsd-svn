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
#include "rdsclient.h"
#include <sstream>

namespace std {

RDSclient::RDSclient()
{
  fd = -1;
  log = 0;
}


RDSclient::~RDSclient()
{
  Close();
}

void RDSclient::SetLogHandler(LogHandler *loghandler)
{
  log = loghandler;
}

int RDSclient::GetFd()
{
  return fd;
}

void RDSclient::SetFd(int NewFd)
{
  fd = NewFd;
}

void RDSclient::Close()
{
  if (fd>=0) close(fd);
  fd = -1;
}


int RDSclient::CheckEvents(RDSsourceList* psrclist)
{
  int i=0;
  while (i<psrclist->size()){
    RDSsource* src = psrclist->at(i);
    if (src){
      rds_events_t requested_events = (src->Data.GetAllEvents()); // & get_event_mask(i));
      if (requested_events){
        ostringstream msg;
        msg << "!" << i << ":" << requested_events << endl;
	text_to_send = text_to_send + msg.str();
	if (! send_text()) return -1;
      }
    }
    ++i;
  }
  return 0;  
}


// Process() returns 0 on success, -1 if the client closes, or an error from enum RdsdError
int RDSclient::Process(RDSsourceList* psrclist)
{
  vector<char>buf(128);
  int bytes_read = read(fd,&buf[0],128);
  if (bytes_read == 0) return -1;
  else if (bytes_read>0){
    int i=0;
    while (i<bytes_read){
      switch (buf[i]){
        case 10 :
	case 13 :  if (!cmd.empty()) ExecCmd(psrclist);
		   break;
	default: cmd += buf[i];
	         if (cmd.size()>100) cmd.clear(); //prevent overflow attacks...  
      }
      ++i;
    }
  } 
  return 0;
}

int RDSclient::ExecCmd(RDSsourceList* psrclist)
{
  int src_num;
  string cmd_str;
  long param;
  bool par_valid;
  split_cmd(src_num,cmd_str,param,par_valid);
  uint i;
  ostringstream msg;
  RDSsource* src=0;
  if ((src_num>=0)&&((uint)src_num<psrclist->size())){
    src = psrclist->at(src_num);
  }
  
  if (cmd_str=="esrc"){
    msg << "esrc" << endl;
    i=0;
    while (i<psrclist->size()){
      RDSsource* tmpsrc=psrclist->at(i);
      if (tmpsrc) msg << i << ":" << tmpsrc->GetName() << endl;
      ++i;
    }
  }
  else if (cmd_str=="stat"){
    msg << src_num << ":stat" << endl;
    if (src) msg << src->GetStatusStr() << endl;
  }
  else if (cmd_str=="sevnt"){
    msg << src_num << ":sevnt" << endl;
    if (src){
      if (par_valid){
        while (event_masks.size()<=src_num) event_masks.push_back(0);
	event_masks[src_num]=(rds_events_t)param;
	msg << "OK" << endl;
      } 
      else msg << "#ERROR: Invalid parameter." << endl;
    }
  }
  else if (cmd_str=="gevnt"){
    msg << src_num << ":gevnt" << endl;
    if (src) msg << get_event_mask(src_num) << endl;
  }
  else if (cmd_str=="rtxt"){
    msg << src_num << ":rtxt" << endl;
    if (src) msg << src->Data.GetRadioText() << endl;
  }
  else if (cmd_str=="lrtxt"){
    msg << src_num << ":lrtxt" << endl;
    if (src) msg << src->Data.GetLastRadioText() << endl;
  }
  else if (cmd_str=="rflags"){
    msg << src_num << ":rflags" << endl;
    if (src) msg << src->Data.GetRDSFlags() << endl;
  }
  else if (cmd_str=="pname"){
    msg << src_num << ":pname" << endl;
    if (src) msg << src->Data.GetProgramName() << endl;
  }
  else if (cmd_str=="utcdt"){
    msg << src_num << ":utcdt" << endl;
    if (src) msg << src->Data.GetUTCDateTimeString() << endl;
  }
  else if (cmd_str=="locdt"){
    msg << src_num << ":locdt" << endl;
    if (src) msg << src->Data.GetLocalDateTimeString() << endl;
  }
  else{
    msg << "#ERROR: Illegal command <" << cmd << ">" << endl;
  }
  if ((!src)&&(cmd_str!="esrc")) msg << "#ERROR: Unknown source." << endl;
  msg << "." << endl;
  text_to_send = text_to_send + msg.str();
  send_text();
  cmd.clear();
  return 0;
}

void RDSclient::split_cmd(int& src_num, string& cmd_str, long& param, bool& par_valid)
{
  string numstr;
  string paramstr;
  cmd_str.clear();
  src_num = -1;
  param = 0;
  par_valid = true;
  bool is_num = true;
  bool is_par = false;
  uint i = 0;
  while (i<cmd.size()){
    if (is_num && (cmd[i]>='0')&&(cmd[i]<='9')) numstr += cmd[i];
    else{
      is_num = false;
      if (is_par && (cmd[i]>' ')) paramstr += cmd[i];
      else{
        if (cmd[i] == ' ') is_par=true;
        else if (cmd[i] != ':') cmd_str += cmd[i];
	
      }
    }
    ++i;
  }
  i=numstr.size();
  if ((i>0)&&(i<5)){
    src_num = strtol(numstr.c_str(),0,0);
  }
  i=paramstr.size();
  if ((i>0)&&(i<11)){
    char* endp;
    errno = 0;
    param = strtol(paramstr.c_str(),&endp,0);
    if (endp == paramstr.c_str()) par_valid=false;
      else if (*endp != 0) par_valid=false;
    if (errno) par_valid=false;
  }
}

bool RDSclient::send_text()
{
  if (text_to_send.empty()) return true;
  
  int bytes_sent = write(fd,text_to_send.c_str(),text_to_send.size());
  if ((uint)bytes_sent != text_to_send.size()) return false; //TODO: Test only !!! 
  text_to_send = "";
  return true; 
}

rds_events_t RDSclient::get_event_mask(int src_num)
{
  if ((src_num<0)||(src_num>=event_masks.size())) return 0;
  return event_masks[src_num];
}

void RDSclient::LogMsg(LogLevel prio, string msg)
{
  if (log) log->LogMsg(prio,msg);
}

};