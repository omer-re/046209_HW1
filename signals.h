#ifndef _SIGS_H
#define _SIGS_H

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

extern int waitingPID;
using namespace std;

void TerminateHandler(int signal); // handle the CTRL-C Signal
void StopHandler(int signal); // handle the CTRL-Z Signal
#endif