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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdshandler.h"
#include <csignal>
#include <fcntl.h>
#include <string>
#include <sstream>

using namespace std;

RDShandler handler;

static void sig_proc(int signr)
{
  switch (signr){
    case SIGINT:
    case SIGTERM:
                 handler.Terminate();
		 break;
    case SIGHUP: handler.Reload();
                 break; 
  }
}

static int check_pid_file(string pid_file_name)
{
  int fd,ret;
  FILE *fil;
  int oldpid;
  if ((fil = fopen(pid_file_name.c_str(),"r"))!=0){
    if (fscanf(fil,"%d",&oldpid)>0){
      fclose(fil);
      return oldpid;
    }
    fclose(fil);
  }
  if ((fd = open(pid_file_name.c_str(),O_RDWR | O_CREAT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH))<0) return -1;
  struct flock mylock;
  mylock.l_type   = F_WRLCK;
  mylock.l_start  = 0;
  mylock.l_whence = SEEK_SET;
  mylock.l_len    = 0;
  if (fcntl(fd,F_SETLK,&mylock) < 0) return -2;
  if (ftruncate(fd,0) < 0) return -3;
  char pidbuf[10];
  sprintf(pidbuf,"%d\n",getpid());
  int len = strlen(pidbuf);
  if (write(fd,pidbuf,len) != len) return -4;
  
  int flags;
  if ((flags = fcntl(fd,F_GETFD,0)) < 0) return -5;
  flags |= FD_CLOEXEC;
  if (fcntl(fd,F_GETFD,0) < 0) return -6;
  close(fd);
  return 0;
}

static int daemonize()
{
  pid_t pid;
  if ((pid = fork())<0) return -1;
  else if (pid != 0) exit(0);
  
  setsid();
  chdir("/");
  umask(0);
  return 0;
}

static void clean_exit(int err_code)
{
  unlink(handler.GetPidFilename().c_str());
  ostringstream msg;
  if (err_code) msg << "rdsd terminating with error code " << err_code;
  else msg << "rdsd: normal shutdown.";
  handler.log.LogMsg(LL_INFO,msg.str()); 
  exit(err_code);
}

static void usage()
{
  cout << "Usage: rdsd [Options]" << endl;
  cout << "  Options: -c <file name> : use alternative conf file." << endl;
  cout << "           -d             : fork and run as daemon." << endl;
  cout << "           -h             : show this help and exit." << endl;
  cout << "           -k             : kill a running rdsd." << endl;
  cout << "           -r             : ask running rdsd to reload conf file." << endl;
  cout << "           -v             : output version info and exit." << endl;
}

string conf_file_name = "/etc/rdsd.conf";
int  running_pid = -1;
bool do_daemonize = false;
bool do_kill_rdsd = false;
bool do_reload_rdsd = false;


static void parse_cmdline(int argc, char* argv[])
{
  int opt;
  while ((opt = getopt(argc,argv,"c:dhkrv")) != -1){
    switch (opt) {
      case 'c':  conf_file_name = optarg;
                 break;
      case 'd':  do_daemonize = true;
                 break;
      case 'h':  usage();
                 clean_exit(0);
                 break;
      case 'k':  do_kill_rdsd = true;
                 break;
      case 'r':  do_reload_rdsd = true;
                 break;
      case 'v':  cout << VERSION << endl;
                 clean_exit(0);
                 break;
      default:   usage();
                 clean_exit(1); 
    }
  }
}


int main(int argc, char* argv[])
{
  parse_cmdline(argc,argv);

  signal(SIGINT,sig_proc);
  signal(SIGTERM,sig_proc);
  signal(SIGHUP,sig_proc);
  
  handler.log.SetConsoleLog(true);
  handler.log.SetFileLog(false);
  handler.log.SetLogLevel(LL_DEBUG);
    
  int ret = handler.InitConf(conf_file_name);
  if (ret){
    handler.log.LogMsg(LL_EMERG,"Cannot initialize RDS handler.");
    clean_exit(1);
  }
  else handler.log.LogMsg(LL_DEBUG,"RDS handler initialized.");
  
  ret = check_pid_file(handler.GetPidFilename());
  running_pid = (ret>0) ? ret : -1;

  if (ret<0){
    handler.log.LogMsg(LL_EMERG,"Cannot initialize PID file (already running ?).");
    clean_exit(1);   
  }
    
  if ((do_reload_rdsd || do_kill_rdsd) && (running_pid<0)){
    handler.log.LogMsg(LL_ERR,"Cannot find PID (no rdsd running ?).");
    clean_exit(1);
  }
  
  if (do_reload_rdsd){
    if (kill(running_pid,SIGHUP)<0){
      handler.log.LogMsg(LL_ERR,"Cannot send SIGHUP (stale rdsd.pid file ?).");
      clean_exit(1);
    }
    handler.log.LogMsg(LL_INFO,"SIGHUP sent to rdsd.");
    clean_exit(0);
  }
  
  if (do_kill_rdsd){
    if (kill(running_pid,SIGINT)<0){
      handler.log.LogMsg(LL_ERR,"Cannot send SIGINT (stale rdsd.pid file ?).");
      clean_exit(1);
    }
    handler.log.LogMsg(LL_INFO,"SIGINT sent to rdsd.");
    clean_exit(0);
  }
    
  if (do_daemonize) {
    unlink(handler.GetPidFilename().c_str());
    handler.log.SetConsoleLog(false);
    if (daemonize()<0){
      handler.log.LogMsg(LL_EMERG,"Ooops! Cannot fork()...");
      clean_exit(2);
    } 
    check_pid_file(handler.GetPidFilename());
  }
  
  ret = handler.Init();
  if (ret) clean_exit(ret);
  
  ret = handler.WorkLoop();
  
  clean_exit(ret);
}



