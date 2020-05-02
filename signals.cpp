// signals.c
// contains signal handler funtions
// contains the functions that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"
#include "commands.h"

extern smash smash1;

//********************************************
// function name: TerminateHandler
// Description: CTRL-C signal handler
// Parameters: signal
// Returns: void
//**************************************************************************************
void TerminateHandler(int signal) {
    pid_t currPid = (-1) * smash1.fgJob().getPID();

    //nothing in fg
    if (currPid == 1)
        return;
    int res_kill = kill((-1) * waitingPID, SIGINT);

    //  means there is a command running in the foreground
    if (res_kill == -1) {
        cout << " cannot send signal" << endl;
        return;
    } else {
        cout << "\nsignal SIGINT was sent to pid " << (-1) * currPid << endl;
        smash1.removeFromFG(); // remove job from fg
    }
    int status;
    int w = waitpid(currPid, &status, WUNTRACED);    //check
    if (w == -1) {
        perror("smash error: > waitpid");
    }
}


//********************************************
// function name: StopHandler
// Description: CTRL-Z signal handler
// Parameters: signal
// Returns: void
//**********************************************
void StopHandler(int signal) { // handle the CTRL-Z Signal
    pid_t currPid = smash1.fgJob().getPID();
    //nothing in fg
    if (currPid == -1)
        return;
    int k = kill(currPid, signal);
    if (k == -1) {
        cout << " cannot send signal" << endl;
        return;
    } else {
        cout << "\nsignal SIGTSTP was sent to pid " << currPid << endl;
        job FG = smash1.fgJob();
        FG.jobSuspended();
        // add to jobList
        smash1.addJob(FG);
        // remove job from fg
        smash1.removeFromFG();
    }
}
