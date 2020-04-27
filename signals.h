#ifndef _SIGS_H
#define _SIGS_H

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <vector>
#include <list>
//new libs
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include <functional>
#include "commands.h"


extern int waitingPID;
extern list<job> jobs;
using namespace std;

void TerminateHandler(int signal); // handle the CTRL-C Signal
void StopHandler(int signal); // handle the CTRL-Z Signal

//void get_smash1_to_signals(smash &smash1);

#endif