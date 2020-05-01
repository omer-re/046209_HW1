// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

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
    // cout << "TerminateHandler" << "  waitingpid=  " << waitingPID << "  " << endl;
    pid_t currPid = (-1) * smash1.fgJob().getPID();
    // TODO maybe the -1 here makes problems?
    //cout << "TerminateHandler" <<  "  currpid=  "<< currPid<< "  "<< endl;
    if (currPid == -1) //nothing in fg
        return;
    int res_kill = kill((-1) * waitingPID, SIGINT);
    if (res_kill == -1) {  //  means there is a command running in the foreground
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
//**************************************************************************************
void StopHandler(int signal) { // handle the CTRL-Z Signal
    cout << endl;
    pid_t currPid = smash1.fgJob().getPID();
    //cout << "WaitingPID is: " << waitingPID << endl;
    //cout << "currPID is: " << currPid << endl;
    if (currPid == -1) //nothing in fg
        return;
    int k = kill(currPid, signal);
    if (k == -1) {
        cout << " cannot send signal" << endl;
        return;
    } else {
        cout << "\nsignal SIGTSTP was sent to pid " << currPid << endl;
        job FG = smash1.fgJob();
        FG.jobSuspended();
        smash1.addJob(FG);// add to jobList
        smash1.removeFromFG(); // remove job from fg
    }
}
