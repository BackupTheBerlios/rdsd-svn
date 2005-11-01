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
#include "rdsqueryhandler.h"
#include <iostream>
#include <cstdlib>
#include <vector>

namespace std {

RdsQueryHandler::RdsQueryHandler()
{
  handle = 0;
  src_num = -1;
  show_debug_on_error = true;
}


RdsQueryHandler::~RdsQueryHandler()
{
}

void RdsQueryHandler::ShowError(int rds_err_num)
{
  if ((rds_err_num>=0)&&(rds_err_num<=RDS_ILLEGAL_TIMEOUT))
    cout << RdsErrorStrings[rds_err_num] << endl;
  else
    cout << "Error #" << rds_err_num << endl;
  if (show_debug_on_error) show_debug();
}

void RdsQueryHandler::ShowEnumSrc()
{
  const size_t buf_size = 2048;
  vector<char> buf(buf_size);
  int ret = rds_enum_sources(handle,&buf[0],buf_size);
  cout << "esrc:";
  if (ret) ShowError(ret);
  else {
    string s(&buf[0]);
    cout << s << endl;
  }
}

void RdsQueryHandler::ShowFlags()
{
  rds_flags_t flags;
  int ret = rds_get_flags(handle,src_num,flags);
  cout << "rflags:";
  if (ret) ShowError(ret);
  else {
    if (flags & RDS_FLAG_IS_TP) cout << "TP=1 "; else cout << "TP=0 ";
    if (flags & RDS_FLAG_IS_TA) cout << "TA=1 "; else cout << "TA=0 ";
    if (flags & RDS_FLAG_IS_MUSIC) cout << "MUSIC=1 "; else cout << "MUSIC=0 ";
    if (flags & RDS_FLAG_IS_STEREO) cout << "STEREO=1 "; else cout << "STEREO=0 ";
    if (flags & RDS_FLAG_IS_ARTIFICIAL_HEAD) cout << "AH=1 "; else cout << "AH=0 ";
    if (flags & RDS_FLAG_IS_COMPRESSED) cout << "COMP=1 "; else cout << "COMP=0 ";
    if (flags & RDS_FLAG_IS_DYNAMIC_PTY) cout << "DPTY=1 "; else cout << "DPTY=0 ";
    if (flags & RDS_FLAG_TEXT_AB) cout << "AB=1"; else cout << "AB=0";
    cout << endl;
  }
}

void RdsQueryHandler::ShowPIcode()
{
  int result;
  int ret = rds_get_pi_code(handle,src_num,result);
  cout << "picode:";
  if (ret) ShowError(ret);
  else cout << result << endl;
}

void RdsQueryHandler::ShowPTYcode()
{
  int result;
  int ret = rds_get_pty_code(handle,src_num,result);
  cout << "ptype:";
  if (ret) ShowError(ret);
  else cout << result << endl;
}

void RdsQueryHandler::ShowProgramName()
{
  char buf[9];
  int ret = rds_get_program_name(handle,src_num,buf);
  buf[8] = 0;
  cout << "pname:";
  if (ret) ShowError(ret);
  else {
    string result(buf);
    cout << result << endl;
  }
}

void RdsQueryHandler::ShowRadioText()
{
  char buf[65];
  int ret = rds_get_radiotext(handle,src_num,buf);
  buf[64] = 0;
  cout << "rtxt:";
  if (ret) ShowError(ret);
  else {
    string result(buf);
    cout << result << endl;
  }
}

void RdsQueryHandler::ShowLastRadioText()
{
  char buf[65];
  int ret = rds_get_last_radiotext(handle,src_num,buf);
  buf[64] = 0;
  cout << "lrtxt:";
  if (ret) ShowError(ret);
  else {
    string result(buf);
    cout << result << endl;
  }
}


void RdsQueryHandler::ShowLocalDateTime()
{
  char buf[256];
  int ret = rds_get_local_datetime_string(handle,src_num,buf);
  buf[255] = 0;
  cout << "locdt:";
  if (ret) ShowError(ret);
  else {
    string result(buf);
    cout << result << endl;
  }
}

void RdsQueryHandler::ShowUTCdateTime()
{
  char buf[256];
  int ret = rds_get_utc_datetime_string(handle,src_num,buf);
  buf[255] = 0;
  cout << "utcdt:";
  if (ret) ShowError(ret);
  else {
    string result(buf);
    cout << result << endl;
  }
}

void RdsQueryHandler::show_debug()
{
  unsigned int buf_size = 0;
  rds_get_debug_text(handle,0,buf_size); // query required size
  if (buf_size>0){
    vector<char> buf(buf_size);
    rds_get_debug_text(handle,&buf[0],buf_size);
    string s(buf.begin(),buf.begin()+buf_size);
    cerr << s << endl;
  }
  else cerr << "(No debug messages)" << endl;
}


}
