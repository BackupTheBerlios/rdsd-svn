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
#ifndef STDRDSSOURCE_H
#define STDRDSSOURCE_H

#include <string>
#include <vector>
#include "confsection.h"
#include "loghandler.h"
#include "rdsdecoder.h"


namespace std {

/**
Encapsulates the reading of raw RDS data and the decoding of it. One object of 
this class is created for each input source.

@author Hans J. Koch
*/

enum SourceStatus {SRCSTAT_OK, SRCSTAT_WAIT, SRCSTAT_CLOSED};
enum SourceType {SRCTYPE_NONE, SRCTYPE_RADIODEV, SRCTYPE_FILE, SRCTYPE_I2CDEV};

class RDSsource{
public:
  RDSsource();
  ~RDSsource();
  int Init(ConfSection* sect);
  string GetName();
  string GetPath();
  void SetLogHandler(LogHandler *loghandler);
  string GetStatusStr();
  int GetFd();
  int Open();
  void Close();
  int Process();
  RDSdecoder Data;  
private:
  string srcname;
  string srcpath;
  SourceType src_type;
  int fd;
  SourceStatus status;
  LogHandler *log;
  void LogMsg(LogLevel prio, string msg);
};

typedef vector<RDSsource*> RDSsourceList;

};

#endif