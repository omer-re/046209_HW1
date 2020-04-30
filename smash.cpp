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
//#include <cstring>
#include <iostream>
#include <sstream>

#define MAX_LINE_SIZE 80
#define MAXARGS 20
using namespace std;

char *L_Fg_Cmd;
//void *jobs = NULL; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)
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

    // get_smash1_to_signals(smash1);
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



    //chld.sa_flags = SA_NOCLDSTOP;
    //chld.sa_handler = &catchSigchld;
    //chld.sa_flags = SA_NOCLDSTOP;
    //sigaction(SIGCHLD, &chld, NULL);
    /************************************/

    /************************************/
    // Init globals
    L_Fg_Cmd = (char *) malloc(sizeof(char) * (MAX_LINE_SIZE + 1));
    if (L_Fg_Cmd == NULL)
        exit(-1);
    L_Fg_Cmd[0] = '\0';
    waitingPID = 0;
    suspended_counter = 0;
    job_counter = 0;
    if (!(prev_path = realpath(".", NULL))) {
        perror("pwd");
    }

    while (1) {
        printf("smash > ");
        lineSize[0] = 0;
        fgets(lineSize, MAX_LINE_SIZE, stdin);
        //smash1.updateJobs();
        strcpy(cmdString, lineSize);
        cmdString[strlen(lineSize) - 1] = '\0';
        // perform a complicated Command

        //update status
        list<job> jobs = smash1.GetJobs();
        for (list<job>::iterator it = jobs.begin(); it != jobs.end(); ++it) {
            int status;
            int result = waitpid(it->getPID(), &status, WNOHANG);
            if (result == -1) {
                perror("error:");
                exit(1);
            }

            if (result == it->getPID()) {
                int jpid = it->getPID();
                smash1.eraseJobFromList(jpid);
            }
        }

        cout << "\nline 109\n";
        if (!ExeComp(lineSize)) continue;
        cout << "\nline 111\n";
        // background command
        if (!BgCmd(lineSize, prev_path)) continue;
        // built in commands
        cout << "\nline 115\n";
        ExeCmd(lineSize, cmdString, false, prev_path);
        cout << "\nline 117\n";
        smash1.addToHistory(cmdString); //add command to history list
        cout << "\nline 119\n";
        /* initialize for next line read*/
        lineSize[0] = '\0';
        cmdString[0] = '\0';
        cout << "\nline 119 : Finished loop\n";

    }

    return 0;
}
