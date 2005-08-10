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
#include "rdsdecoder.h"
#include <sstream>
#include <iostream> //test only

namespace std {

RDSdecoder::RDSdecoder()
{
  events = 0;
  next_expected_block = 0;
  last_block_num = -1;
  group_type = GROUP_UNKNOWN;
  PIcode = -1;
  PTYcode = -1;
  program_name.resize(8,'\r');
  radio_text_buf.resize(64,'\r');
  utc_datetime_str   = "2000/1/1 00:00:00";
  local_datetime_str = "2000/1/1 00:00:00";
  tmc_type = TMC_UNKNOWN;
}

RDSdecoder::~RDSdecoder()
{

}

rds_events_t RDSdecoder::GetAllEvents()
{
  return events;
}
  
void RDSdecoder::SetAllEvents(rds_events_t evts)
{
  events = evts;
}
 

// AddBytes() assumes that the first byte in Buf is the beginning of a block
// and that Buf.size() is a multiple of 3
void RDSdecoder::AddBytes(CharBuf* Buf)
{
  uint i=0;
  int index;
  while (i<Buf->size()){
    int b0 = Buf->at(i);
    int b1 = Buf->at(i+1);
    int b2 = Buf->at(i+2);
    i+=3;
    if (! is_valid_block(b2)) continue;
    int blocknum = b2 & 0x03; // What's the differnce between "Received Offset"
                              // and "Offset Name" in V4L2 spec ???
    if (blocknum == last_block_num) continue;
    last_block_num = blocknum;
    if (blocknum == next_expected_block){
      switch (blocknum){
        case 0: set_pi_code((b1 << 8) | b0); 
	        break;
	case 1: group_type = (RDSGroupType)(b1 >> 3);
		set_rds_flag(RDS_FLAG_IS_TP,(b1 & 0x04));
		set_pty_code(((b1 << 3) & 0x18) | ((b0 >> 5) & 0x07));
		block1_lower5 = (b0 & 0x1F);
		switch (group_type){
	          case GROUP_0A:
		  case GROUP_0B:  set_rds_flag(RDS_FLAG_IS_TA,(block1_lower5 & 0x10));
		                  set_rds_flag(RDS_FLAG_IS_MUSIC,(block1_lower5 & 0x08));
		                  break;
		  case GROUP_2A:
		  case GROUP_2B:  set_rds_flag(RDS_FLAG_TEXT_AB,(block1_lower5 & 0x10));
		                  break;
		  case GROUP_8A:  tmc_type = (TMCtype)((block1_lower5 & 0x18) >> 3);
		                  break;
		  default: ;
				  
	        }
	        break;
	case 2: switch (group_type){
	          case GROUP_0A:
		  case GROUP_0B:  
		                  break;
		  case GROUP_2A:  index = (block1_lower5 & 0x0F) << 2;
		                  set_radiotext(index,b1,b0);
		                  break;
		  case GROUP_2B:  set_pi_code((b1 << 8) | b0);
		                  break;
		  case GROUP_4A:  jul_date = ((block1_lower5 & 0x03) << 15) | (b1 << 7) | (b0 >> 1);
		                  utc_hour = (b0 & 0x01) << 4;
		                  break;
		  case GROUP_8A:  tmc_event = ((b1 & 0x07) << 8) | b0;
		                  break;
		  default: ;
				  
	        }
	        break;
	case 3: switch (group_type){
	          case GROUP_0A:
		  case GROUP_0B:  switch (block1_lower5 & 0x03){
				    case 0: set_prog_name(0,b1,b0);
					    set_rds_flag(RDS_FLAG_IS_DYNAMIC_PTY,(block1_lower5 & 0x04));
				            break;
				    case 1: set_prog_name(2,b1,b0);
					    set_rds_flag(RDS_FLAG_IS_COMPRESSED,(block1_lower5 & 0x04));
				            break;
				    case 2: set_prog_name(4,b1,b0);
					    set_rds_flag(RDS_FLAG_IS_ARTIFICIAL_HEAD,(block1_lower5 & 0x04));
				            break;
				    case 3: set_prog_name(6,b1,b0);
					    set_rds_flag(RDS_FLAG_IS_STEREO,(block1_lower5 & 0x04));
				            break;    
				  }
		                  break;
		  case GROUP_2A:  index = (block1_lower5 & 0x0F) << 2;
		                  set_radiotext(index+2,b1,b0);
		                  break;
		  case GROUP_2B:  index = (block1_lower5 & 0x0F) << 1;
		                  set_radiotext(index,b1,b0);
		                  break;
		  case GROUP_4A:  utc_hour |= ((b1 & 0xF0) >> 4);
		                  utc_minute = ((b1 & 0x0F) << 2)|((b0 & 0xC0) >> 6);
				  utc_offset = (b0 & 0x1F);
				  if (b0 & 0x20) utc_offset = -utc_offset;
				  set_datetime_strings();
		                  break;
		  case GROUP_8A:  tmc_location = (b1 << 8) | b0;
		                  break;
		  
		  default: ;
				  
	        }
                break;
      }
      if (++next_expected_block > 3) next_expected_block = 0;
    }
    else next_expected_block = 0;
  }
}

rds_flags_t RDSdecoder::GetRDSFlags()
{
  return rds_flags;
}

const string& RDSdecoder::GetRadioText()
{
  radio_text.clear();
  bool sth_found = false;
  int i = 0;
  while (i < radio_text_buf.size()){
    if (radio_text_buf[i] == '\r'){
      if (sth_found) break;
      else radio_text.push_back(' ');
    }
    else{
      sth_found = true;
      radio_text.push_back(radio_text_buf[i]); 
    }
    ++i;
  }
  if(! sth_found) radio_text.clear();
  return radio_text;
}

const string& RDSdecoder::GetLastRadioText()
{
  return last_radio_text;
}

const string& RDSdecoder::GetProgramName()
{
  return program_name;
}

const string& RDSdecoder::GetUTCDateTimeString()
{
  return utc_datetime_str;
}

const string& RDSdecoder::GetLocalDateTimeString()
{
  return local_datetime_str;
}

int RDSdecoder::GetPIcode()
{
  return PIcode;
}

int RDSdecoder::GetPTYcode()
{
  return PTYcode;
}

bool RDSdecoder::is_valid_block(int b2)
{
  if ((b2 & 0x80)!=0){
    //TODO: Add some error statistics here...
    return false;
  }
  return true;
}

void RDSdecoder::set_event(rds_events_t evnt)
{
  events |= evnt;
}

void RDSdecoder::set_rds_flag(rds_flags_t flag, bool new_state)
{
  bool current_state = (rds_flags & flag);
  if (current_state != new_state) set_event(RDS_EVENT_FLAGS);
  if (new_state) rds_flags |= flag;
  else rds_flags &= (!flag);
}

void RDSdecoder::set_pi_code(int new_code)
{
  if (new_code != PIcode) set_event(RDS_EVENT_PI_CODE);
  PIcode = new_code;
}

void RDSdecoder::set_pty_code(int new_code)
{
  if (new_code != PTYcode) set_event(RDS_EVENT_PTY_CODE);
  PTYcode = new_code;
}

void RDSdecoder::set_prog_name(int first_index, char c1, char c2)
{
  if ((program_name[first_index] != c1)||(program_name[first_index+1] != c2)){
    set_event(RDS_EVENT_PROGRAMNAME);
    program_name[first_index]   = c1;
    program_name[first_index+1] = c2;
  }
}

void RDSdecoder::set_radiotext(int first_index, char c1, char c2)
{
  if (first_index == 0) set_last_radiotext(); 
  if ((radio_text_buf[first_index] != c1)||(radio_text_buf[first_index+1] != c2)){
    set_event(RDS_EVENT_RADIOTEXT);
    radio_text_buf[first_index]   = c1;
    radio_text_buf[first_index+1] = c2;
  }
  if ((c1=='\r')||(c2=='\r')) set_last_radiotext();
}

void RDSdecoder::set_last_radiotext()
{
  string temp;
  int i=0;
  while (i<radio_text_buf.size()){
    if (radio_text_buf[i] == '\r') break;
    temp.push_back(radio_text_buf[i]);
    radio_text_buf[i] = '\r';
    ++i;
  }
  //cout << temp << endl;
  if (temp != last_radio_text){
    last_radio_text = temp;
    set_event(RDS_EVENT_LAST_RADIOTEXT);
  }
}

void RDSdecoder::set_datetime_strings()
{
  const int month_days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
  int Y,M,D,K;
  Y = (int)(((double)jul_date - 15078.2)/365.25);
  M = (int)(((jul_date - 14956.1)-(int)(Y*365.25))/30.6001);
  D = jul_date - 14956 - (int)(Y*365.25) - (int)(M * 30.6001);
  if ((M == 14)||(M == 15)) K=1; else K=0;
  Y = Y + K + 1900;
  M = M - 1 - (K*12);
  ostringstream utcss;

  utcss << Y << "/" << D << "/" << M << " ";
  utcss << utc_hour << ":" << utc_minute << ":00";

  utc_datetime_str = utcss.str();

  int loc_hour = utc_hour; 
  int loc_min  = utc_minute + (utc_offset*30);

  while (loc_min < 0){
    loc_min += 60;
    --loc_hour;
  }
  while (loc_min >= 60){
    loc_min -= 60;
    ++loc_hour;
  }
  if (loc_hour < 0){
    loc_hour += 24;
    --D;
    if (D<1){
      --M;
      if (M<1){
        M=12;
	--Y;
      }
      D = month_days[M-1];
      if (((Y % 4)==0)&&(M==2)) D=29;
    }
  }
  if (loc_hour >= 24){
    loc_hour -= 24;
    ++D;
    int Dmax = (((Y % 4)==0)&&(M==2)) ? 29 : month_days[M-1];
    if (D > Dmax){
      ++M;
      if (M > 12){
        M = 1;
	++Y;
      }
    }
  }

  ostringstream locss;
  
  locss << Y << "/" << D << "/" << M << " ";
  locss << loc_hour << ":" << loc_min << ":00";

  local_datetime_str = locss.str();

  set_event(RDS_EVENT_DATETIME);
}



};
