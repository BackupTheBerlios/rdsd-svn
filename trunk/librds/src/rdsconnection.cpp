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

namespace std {

RDSconnection::RDSconnection()
{
}


RDSconnection::~RDSconnection()
{
}

int RDSconnection::Open(string path, int conn_type)
{

  return 0;
}

int RDSconnection::Close()
{

  return 0;
}

int RDSconnection::SetEventMask(rds_events_t evnt_mask)
{

  return 0;
}

int RDSconnection::GetEventMask(rds_events_t &evnt_mask)
{

  return 0;
}

int RDSconnection::GetEvent(rds_events_t &events)
{

  return 0;
}

int RDSconnection::GetFlags(rds_flags_t &flags)
{

  return 0;
}

int RDSconnection::GetPTYcode(int &pty_code)
{

  return 0;
}

int RDSconnection::GetPIcode(int &pi_code)
{

  return 0;
}

int RDSconnection::GetProgramName(char* buf)
{

  return 0;
}

int RDSconnection::GetRadiotext(char* buf)
{

  return 0;
}

int RDSconnection::GetLastRadiotext(char* buf)
{

  return 0;
}

int RDSconnection::GetUTCDateTimeString(char* buf)
{

  return 0;
}

int RDSconnection::GetLocalDateTimeString(char* buf)
{

  return 0;
}


};
