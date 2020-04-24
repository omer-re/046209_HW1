// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"
#include "commands.h"

smash *smash1_ptr;

void get_smash1_to_signals(smash &smash1) {
    smash1_ptr = &smash1;
}


//********************************************
// function name: TerminateHandler
// Description: CTRL-C signal handler
// Parameters: signal
// Returns: void
//**************************************************************************************
void TerminateHandler(int signal) {
    cout << "TerminateHandler" << "  waitingpid=  " << waitingPID << "  " << endl;
    cout << endl;
    if (waitingPID) {  //  means there is a command running in the foreground
        if (kill(waitingPID, SIGINT) == -1)
            return;
        waitingPID = 0;
    }
}
//********************************************
// function name: StopHandler
// Description: CTRL-Z signal handler
// Parameters: signal
// Returns: void
//**************************************************************************************
void StopHandler(int signal) { // handle the CTRL-Z Signal
    cout << "StopHandler" << "  waitingpid=  " << waitingPID << "  " << endl;
    cout << endl;
    if (waitingPID) {  //  means there is a command running in the foreground
        if (kill(waitingPID, SIGTSTP) == -1)
            return;
        list<job>::iterator it;
        // find the command number of waiting PID
        if ((it = smash1_ptr->getJobFromPID(waitingPID)) != smash1_ptr->getListEnd()) {
            it->jobSuspended();
            //break;
        }
        waitingPID = 0;
    }

}