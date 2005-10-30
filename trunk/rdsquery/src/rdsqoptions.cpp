/***************************************************************************
 *   Copyright (C) 2005 by Hans J. Koch                                    *
 *   koch@users.berlios.de                                               *
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
#include "rdsqoptions.h"
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sstream>

namespace std {

RdsqOptions::RdsqOptions()
{
  record_count = 1;
  conn_type = CONN_TYPE_UNIX;
  server_name = "/var/tmp/rdsd.sock";
  tcpip_port = 4321;
  source_num = 0;
  event_mask = 0;
  have_opt_s = false;
  have_opt_t = false;
  have_opt_u = false;
}


RdsqOptions::~RdsqOptions()
{
}


bool RdsqOptions::ProcessCmdLine(int argc, char *argv[])
{
  char option;
  int itmp;
  rds_events_t evnt_tmp;

  while ( (option = getopt(argc,argv,"c:hvs:u:t:")) != EOF ) {
    switch (option){
      case 'c' :  if (try_str_to_int(optarg,itmp)) record_count=itmp;
                  else { show_usage(); return false; }
                  break;
      case 's' :  if (have_opt_u){ show_usage(); return false; }
		  server_name = optarg;
      		  have_opt_s = true;
                  break;
      case 'p' :  if (have_opt_u){ show_usage(); return false; }
		  if (try_str_to_int(optarg,itmp)) tcpip_port=itmp;
                  else { show_usage(); return false; }
             	  have_opt_p = true;
      		  break;
      case 'u' :  if (have_opt_s){ show_usage(); return false; }
		  if (have_opt_p){ show_usage(); return false; }
		  server_name = optarg;
      		  have_opt_u = true;
                  break;
      case 't' :  if (try_parse_types(optarg,evnt_tmp)) event_mask=evnt_tmp;
      		  else { show_usage(); return false; }
      		  have_opt_t = true;
                  break;
      case 'h' :  show_usage();
                  exit(0);
                  break;
      case 'v' :  show_version();
                  exit(0);
                  break;
      default  :  show_usage();
                  return false;
    }
  }
  if (! have_opt_t) { show_usage(); return false; }
  if ((have_opt_p)&&(!have_opt_s)){ show_usage(); return false; }
  return true;
}


void RdsqOptions::show_usage()
{
  cout << "Usage:" << endl;
  cout << "(TODO...)" << endl;
}

void RdsqOptions::show_version()
{
  cout << "0.0.1" << endl;
}

bool RdsqOptions::try_str_to_int(char *s, int &result)
{
  if (! s) return false;
  istringstream iss(s);
  if (iss >> result) return true;
  return false;
}

int RdsqOptions::find_cmd_num(const string& S)
{
  int i=1;
  while (i<RDS_CMD_COUNT){
    if (RdsCommands[i]==S) return i;
    i++;
  }
  return 0;
}

bool RdsqOptions::try_parse_types(char *s, rds_events_t &result)
{
  result = 0;
  if (! s) return false;
  string S(s);
  string cmd;
  int i=0;
  while (i<S.size()){
    if (S[i] != ',') cmd.push_back(S[i]);
    if ((S[i] == ',')||(i==(S.size()-1))) {
      int cmd_num = find_cmd_num(cmd);
      if (cmd_num == RDS_CMD_NONE) return false;
      switch (cmd_num){
        case RDS_CMD_FLAGS         : result |= RDS_EVENT_FLAGS; break;
        case RDS_CMD_PI_CODE       : result |= RDS_EVENT_PI_CODE; break;
        case RDS_CMD_PTY_CODE      : result |= RDS_EVENT_PTY_CODE; break;
        case RDS_CMD_PROGRAMNAME   : result |= RDS_EVENT_PROGRAMNAME; break;
        case RDS_CMD_LOCDATETIME   : result |= RDS_EVENT_DATETIME; break;
        case RDS_CMD_UTCDATETIME   : result |= RDS_EVENT_DATETIME; break;
        case RDS_CMD_RADIOTEXT     : result |= RDS_EVENT_RADIOTEXT; break;
        case RDS_CMD_LAST_RADIOTEXT: result |= RDS_EVENT_LAST_RADIOTEXT; break;
        case RDS_CMD_TMC           : result |= RDS_EVENT_TMC; break;
      }
      cmd = "";
    }
    i++;
  }
  return true;
}

}
