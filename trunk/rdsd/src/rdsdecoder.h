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
#ifndef STDRDSDECODER_H
#define STDRDSDECODER_H

#include <vector>
#include <string>
#include <librds.h>

namespace std {

/**
This class is used to decode raw RDS data received from a RDS source.

@author Hans J. Koch
*/

enum RDSGroupType {GROUP_0A,GROUP_0B,GROUP_1A,GROUP_1B,GROUP_2A,GROUP_2B,
                   GROUP_3A,GROUP_3B,GROUP_4A,GROUP_4B,GROUP_5A,GROUP_5B,
		   GROUP_6A,GROUP_6B,GROUP_7A,GROUP_7B,GROUP_8A,GROUP_8B,
		   GROUP_9A,GROUP_9B,GROUP_10A,GROUP_10B,GROUP_11A,GROUP_11B,
		   GROUP_12A,GROUP_12B,GROUP_13A,GROUP_13B,GROUP_14A,GROUP_14B,
		   GROUP_15A,GROUP_15B,GROUP_UNKNOWN};
		   
enum TMCtype {TMC_GROUP,TMC_SINGLE,TMC_SYSTEM,TMC_TUNING,TMC_UNKNOWN};

typedef vector<unsigned char> CharBuf;

class RDSdecoder{
public:
  RDSdecoder();
  ~RDSdecoder();
  rds_events_t GetAllEvents();
  void SetAllEvents(rds_events_t evts);
  void AddBytes(CharBuf* Buf);
  rds_flags_t GetRDSFlags();
  const string& GetRadioText();
  const string& GetLastRadioText();
  const string& GetProgramName();
  const string& GetUTCDateTimeString();
  const string& GetLocalDateTimeString();
  int GetPIcode();
  int GetPTYcode();
private:
  rds_events_t events;
  string radio_text_buf;
  string radio_text;
  string last_radio_text;
  string program_name;
  string utc_datetime_str;
  string local_datetime_str;
  int PIcode;
  int PTYcode;
  int jul_date;
  int utc_hour;
  int utc_minute;
  int utc_offset;
  TMCtype tmc_type;
  int tmc_event;
  int tmc_location;
  rds_flags_t rds_flags;
  bool is_valid_block(int b2);
  void set_event(rds_events_t evnt);
  void set_rds_flag(rds_flags_t flag, bool new_state);
  void set_pi_code(int new_code);
  void set_pty_code(int new_code);
  void set_prog_name(int first_index, char c1, char c2);
  void set_radiotext(int first_index, char c1, char c2);
  void set_last_radiotext();
  void set_datetime_strings();
  int next_expected_block;
  int last_block_num;
  RDSGroupType group_type;
  int block1_lower5;
};

};

#endif
