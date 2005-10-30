/***************************************************************************
 *   Copyright (C) 2005 by Hans J. Koch                                    *
 *   koch@hjk-az.de                                                        *
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
#ifndef STDRDSDCOMMANDLIST_H
#define STDRDSDCOMMANDLIST_H

#include <string>
#include <map>

enum RdsdCommandStatus {RCS_VALID,RCS_WAITING,RCS_REQUEST_NEEDED};

namespace std {

/**
RdsdCommand is a small class used in RdsdCommandList. It is used to store
information about a command sent to rdsd.

@author Hans J. Koch
*/

class RdsdCommand{
public:
  //! The constructor
  RdsdCommand() { status=RCS_REQUEST_NEEDED; };
  //! The destructor
  ~RdsdCommand() { };
  //! Get the current status of the command
  RdsdCommandStatus GetStatus(){ return status; };
  //! Set the current status of the command
  void SetStatus(RdsdCommandStatus new_status){ status = new_status; };
  //! Get stored data
  const string& GetData(){ return data; }
  //! Store data string
  void SetData(const string& new_data){ data = new_data; };
private:
  RdsdCommandStatus status;
  string data;
};

typedef map<string,RdsdCommand*> RdsdCommandMap;

/**
RdsdCommandList is a class used in librds. It stores status and data of commands
that were sent to rdsd.

@author Hans J. Koch
*/
class RdsdCommandList{
public:
  //! The constructor
  RdsdCommandList();
  //! The destructor
  ~RdsdCommandList();
  //! Clear the list, free allocated ressources
  void Clear();
  //! Find information about a command string
  RdsdCommand* Find(const string& CmdStr);
  //! Find information about a command string. If no object is found, add a new one.
  RdsdCommand* FindOrAdd(const string& CmdStr);
private:
  RdsdCommandMap CmdMap;

};

};

#endif
