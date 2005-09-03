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
#include "rdsdcommandlist.h"

namespace std {

RdsdCommandList::RdsdCommandList()
{
}


RdsdCommandList::~RdsdCommandList()
{
}

void RdsdCommandList::Clear()
{
  map<const string&,RdsdCommand*>::iterator it;
  for (it=CmdMap.begin(); it != CmdMap.end(); it++){
    if (it){
      if (*it) delete (*it);
    }
  }
  CmdMap.clear();
}

RdsdCommand* RdsdCommandList::Find(const string& CmdStr)
{
  map<const string&,RdsdCommand*>::iterator it = CmdMap.find(CmdStr);
  if (it) return *it; else return 0;
}

RdsdCommand* RdsdCommandList::FindOrAdd(const string& CmdStr)
{
  RdsdCommand* result = Find(CmdStr);
  if (! result){
    result = new RdsdCommand;
    CmdMap[CmdStr] = result;
  }
  return result;
}


};
