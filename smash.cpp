/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include <signal.h>
#include "commands.h"
#include "signals.h"

//new libs
#include <string>
#include <iostream>
#include <sstream>

#define MAX_LINE_SIZE 80
#define MAXARGS 20
using namespace std;

char *L_Fg_Cmd;
char lineSize[MAX_LINE_SIZE];
// new fields

smash smash1;
int suspended_counter;
int job_counter;
int waitingPID;


//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[]) {
    char cmdString[MAX_LINE_SIZE];
    char *prev_path;

    //signal declaretions
    //NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
    /* add your code here */
    struct sigaction stop, term;

    term.sa_handler = &TerminateHandler;
    sigfillset(&term.sa_mask);
    term.sa_flags = 0;

    stop.sa_handler = &StopHandler;
    sigfillset(&stop.sa_mask);
    stop.sa_flags = 0;
    /************************************/
    //NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
    //set your signal handlers here
    sigaction(SIGINT, &term, NULL);
    sigaction(SIGTSTP, &stop, NULL);

    /************************************/

    /************************************/
    // Init globals
    L_Fg_Cmd = (char *) malloc(sizeof(char) * (MAX_LINE_SIZE + 1));
    if (L_Fg_Cmd == NULL)
        exit(-1);
    L_Fg_Cmd[0] = '\0';
    waitingPID = 0;
    suspended_counter = 0;
    job_counter = 1;
    if (!(prev_path = realpath(".", NULL))) {
        perror("pwd");
    }

    while (1) {
        printf("smash > ");
        lineSize[0] = 0;
        fgets(lineSize, MAX_LINE_SIZE, stdin);


        //update status
        list<job> jobs = smash1.GetJobs();
        for (list<job>::iterator it = jobs.begin(); it != jobs.end(); ++it) {
            int status;
            int result = waitpid(it->getPID(), &status, WNOHANG);
            if (result == -1) {
                perror("error waitpid");
                exit(1);
            }

            if (result == it->getPID()) {
                int jpid = it->getPID();
                smash1.eraseJobFromList(jpid);
            }
        }

        strcpy(cmdString, lineSize);
        cmdString[strlen(lineSize) - 1] = '\0';
        // perform a complicated Command
        if (!ExeComp(lineSize)) continue;
        // background command
        if (!BgCmd(lineSize, prev_path)) continue;
        // built in commands
        ExeCmd(lineSize, cmdString, false, prev_path);
        smash1.addToHistory(cmdString); //add command to history list
        /* initialize for next line read*/
        lineSize[0] = '\0';
        cmdString[0] = '\0';

    }

    return 0;
}
