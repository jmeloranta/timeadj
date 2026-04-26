/*
 * System clock adjustment command line tool. 
 *
 * Needs to be run as root (setuid; chown root timeadjx; chmod u+s timeadjx).
 *
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define TD_TOOL "/usr/bin/timedatectl"

void adj_time(double offset) {

  struct timeval cur_time;
  double ti;

  if(gettimeofday(&cur_time, 0) < 0) {
    fprintf(stderr, "gettimeofday() failed.\n");
    exit(1);
  }
  ti = ((double) cur_time.tv_sec) + ((double) cur_time.tv_usec) / 1.0E6;
  ti += offset;
  cur_time.tv_sec = (time_t) ti;
  cur_time.tv_usec = (ti - (double) cur_time.tv_sec) * 1E6;
  if(settimeofday(&cur_time, 0) < 0) {
    fprintf(stderr, "settimeofday() failed.\n");
    exit(1);
  }
}

void time_auto(int setting, char *envp[]) { // 0 = turn NTP off, 1 = turn NTP on

  char *args[4];

  args[0] = TD_TOOL;
  args[1] = "set-ntp";
  if(setting)
    args[2] = "1";
  else
    args[2] = "0";
  args[3] = NULL;
  execve(TD_TOOL, args, envp);
}

int main(int argc, char *argv[], char *envp[]) {

  if(argc != 2) {
    fprintf(stderr, "Usage: timeadjx [#|o|f].\n");
    return 1;
  }
  setresuid(0, 0, 0);
  setresgid(0, 0, 0);
  switch(*argv[1]) {
    case 'o':
      time_auto(1, envp);
      break;
    case 'f':
      time_auto(0, envp);
      break;
    default:
      adj_time(atof(argv[1]));
  }
  return 0;
}
