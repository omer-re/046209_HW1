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
/**void TerminateHandler(int signal) {
    pid_t currPid = (-1)*smash1.fgJob().getPID();
    cout << "\nWaitingPID is: " << waitingPID << endl;
    cout << "currPID is: " << currPid << endl;
    if (currPid == -1) //nothing in fg
        return;

    int k = kill(currPid, SIGINT);
    cout << "k =" << k << endl;

    int status;
    pid_t result = waitpid(currPid, &status, WNOHANG);
    cout << "waitpid result is: " << result << endl;

    if (result == 0) {
        // Child still alive
        cout << "Child is still alive" << endl;
        cout << "WaitingPID is: " << waitingPID << endl;
        cout << "currPID is: " << currPid << endl;

    } else if (result == -1) {
        // Error
        cout << "Child error" << endl;

    } else {
        // Child exited
        cout << "Child exited" << endl;

    }

    if (k == -1) {
        cout << " cannot send signal" << endl;
        return;
    } else {
        cout << "signal SIGINT was sent to pid " << currPid << endl;
        //smash1.removeFromFG(); // remove job from fg
        // anything else??
    }
}

//********************************************
// function name: StopHandler
// Description: CTRL-Z signal handler
// Parameters: signal
// Returns: void
//**************************************************************************************
void StopHandler(int signal) { // handle the CTRL-Z Signal
    pid_t currPid = smash1.fgJob().getPID();
    cout << "WaitingPID is: " << waitingPID << endl;
    cout << "currPID is: " << currPid << endl;
    if (currPid == -1) //nothing in fg
        return;
    int k = kill(currPid, signal);
    if (k == -1) {
        cout << " cannot send signal" << endl;
        return;
    } else {
        cout << "signal SIGTSTP was sent to pid " << currPid << endl;
        job FG = smash1.fgJob();
        FG.jobSuspended();
        smash1.addJob(FG);// add to jobList
        smash1.removeFromFG(); // remove job from fg
    }

}**/


//********************************************
// function name: TerminateHandler
// Description: CTRL-C signal handler
// Parameters: signal
// Returns: void
//**************************************************************************************
void TerminateHandler(int signal) {
   // cout << "TerminateHandler" << "  waitingpid=  " << waitingPID << "  " << endl;
    pid_t currPid = (-1) * smash1.fgJob().getPID();
    //cout << "TerminateHandler" <<  "  currpid=  "<< currPid<< "  "<< endl;
    if (currPid == -1) //nothing in fg
        return;
    int res_kill = kill((-1) * waitingPID, SIGINT);
    if (res_kill == -1) {  //  means there is a command running in the foreground
        cout << " cannot send signal" << endl;
        return;
    } else {
        cout << "signal SIGINT was sent to pid " << (-1) * currPid << endl;
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

/*
void StopHandler(int signal) { // handle the CTRL-Z Signal
    cout << "StopHandler" << "  waitingpid=  "<< waitingPID<<"  "<<endl;
    cout <<  endl;

    if (waitingPID){  //  means there is a command running in the foreground
        if(kill(waitingPID, SIGTSTP) == -1)
            return;
        list<job>::iterator it;
        // find the command number of waiting PID
        if ((it=smash1.getJobFromPID(waitingPID))!=smash1.getListEnd()){
            it->jobSuspended();
            //break;
        }
        waitingPID = 0;
    }


}
*/
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
        cout << "signal SIGTSTP was sent to pid " << currPid << endl;
        job FG = smash1.fgJob();
        FG.jobSuspended();
        smash1.addJob(FG);// add to jobList
        smash1.removeFromFG(); // remove job from fg
    }
}
