//		commands.c
//********************************************
#include "commands.h"
#include "signals.h"


using namespace std;
extern smash smash1;

bool areFilesEqual(fstream *, fstream *);

int sizeOfFile(fstream *);

//********************************************
// function name: send_sig
// Description: sends "signal" to "pid", and prints out the information if successful
// Parameters: pid, signal to send
// Returns: 0 - success,-1 - failure
//**************************************************************************************
int send_sig(pid_t pid, int signal) {
    int kill_res = kill(pid, signal);
    if (kill_res == 0) {
        printf("smash > signal %s was sent to pid %d\n", strsignal(signal), pid);
        return 0;
    }
    perror("smash error: >");
    return -1;
}

bool has_only_digits(const char *s) {
    std::string Str = std::string(s);
    if (Str.find_first_not_of("0123456789") == string::npos) {
        return true;
    } else return false;
}

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command std::string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(char *lineSize, char *cmdString, bool bg, char *prev_path) {
    char *cmd;
    char *args[MAX_ARG];
    char *pwd;


    const char *delimiters = " \t\n";
    int i = 0, num_arg = 0;
    bool illegal_cmd = false; // illegal command
    cmd = strtok(lineSize, delimiters);
    if (cmd == NULL)
        return 0;
    args[0] = cmd;
    for (i = 1; i < MAX_ARG; i++) {
        args[i] = strtok(NULL, delimiters);
        if (args[i] != NULL)
            num_arg++;

    }
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
    if (!strcmp(cmd, "cd")) {
        //printf("entered cd");
        if (num_arg == 0) {
            printf("smash error: > \"cd\"\n");
        } else if (num_arg == 1) {
            if (!(pwd = realpath(".", NULL))) {
                perror("smash error: >");
            }

            if (!strcmp(args[1], "-")) {
                // chdir(prev_path.c_str());
                chdir(prev_path);
                cout << prev_path << endl;
            } else if (chdir(args[1]) == -1) {
                printf("smash error: > %s - path not found\n", args[1]);
            }

            strcpy(prev_path, pwd);
            free(pwd);

        } else {
            illegal_cmd = true;
        }
    }

        /*************************************************/
    else if (!strcmp(cmd, "pwd")) {
        if (num_arg == 0) {
            if (!(pwd = realpath(".", NULL))) {
                perror("smash error: >");
            } else {
                printf("%s\n", pwd);
                free(pwd);
            }
        } else
            illegal_cmd = true;


    }

        /*************************************************/
    else if (!strcmp(cmd, "history")) {

        if (num_arg == 0) {
            smash1.printHist();
        } else
            illegal_cmd = true;

    }
        /*************************************************/
    else if (!strcmp(cmd, "jobs")) {
        smash1.updateJobs();
        if (num_arg == 0) {
            smash1.printJobList();

        } else
            illegal_cmd = true;

    }
        /*************************************************/
    else if (!strcmp(cmd, "kill")) {
        if (num_arg == 2) {
            if ((!has_only_digits(args[1] + 1)) || (!has_only_digits(args[2]))) {
                cout << "smash error:> kill job - job does not exist" << endl;
            } else {
                if (args[1][0] != '-') {
                    illegal_cmd = true;
                }
                int JobId = atoi(args[2]);
                if (!smash1.isInJList(JobId)) {    //job doesn't exist
                    cout << "smash error:> kill job - job does not exist" << endl;
                } else {
                    //job exist-
                    int signum = atoi(args[1] + 1);
                    list<job>::iterator it = smash1.getJobFromId(JobId);

                    pid_t Jpid = it->getPID();
                    int k = kill(Jpid, signum);
                    if (k == -1) {
                        cout << "kill job - cannot send signal" << endl;
                    }
                    // else- signal sent
                    cout << "signal " << strdup(sys_siglist[signum]) << " was sent to pid " << Jpid << endl;
                    // }
                    if (signum == SIGCONT) {
                        if (it->isSus() == true) {
                            it->jobUnsuspended();
                        }
                    }
                    if (signum == SIGTSTP) {
                        if (it->isSus() == false) {
                            it->jobSuspended();

                        }
                    }
                }
            }
        } else {
            illegal_cmd = true;

        }

    }
        /*************************************************/
    else if (!strcmp(cmd, "showpid")) {
        if (num_arg != 0)
            illegal_cmd = true;
        else

            printf("smash pid is %d\n", (int) getpid());

    }
        /*************************************************/
    else if (!strcmp(cmd, "fg")) {
        //if arg[2]==NULL find last process that was paused
        //print name of process
        //move process with command number(arg[2]) to bg

        list<job>::iterator iter;
        list<job>::iterator command;
        if ((smash1.isJobListEmpty()) || (num_arg > 1))// if job list is empty
        {
            illegal_cmd = true;
        } else {
            switch (num_arg) {

                case 0:  // handle the last job suspended, move to fg
                    command = smash1.LastInBg();
                    break;

                case 1: // find the relevant job and move it to fg
                    if (!has_only_digits(args[1])) {
                        illegal_cmd = true;
                    }
                    command = smash1.getJobFromId(atoi(args[1]));
                    break;

                default:
                    illegal_cmd = true;
                    break;
            }
            if (command == smash1.getListEnd())    //no such process
                illegal_cmd = true;
            if (!illegal_cmd) //process found
            {

                if (send_sig(command->getPID(), SIGCONT) == 0) // if job was stopped release it
                {
                    //int status;
                    //string name_ = command->getJobName();
                    //int id_ = command->getID();
                    pid_t pid_ = command->getPID();
                    //time_t time_ = command->getTime();

                    //job fg_ = job(pid_, id_, name_, time_);
                    if (command->isSus()) {
                        command->jobUnsuspended();
                    }
                    smash1.setFgJob(*command);
                    printf("%s\n", command->getJobName().c_str());
                    waitingPID = command->getPID();
                    smash1.setLastProcessOnFg(command->getID());

                    waitpid(pid_, NULL, 0);
                    smash1.eraseJobFromList(pid_);
                    // while (waitpid(command->getPID(), NULL, WUNTRACED) == -1);
                    //while (waitpid(command->getPID(), NULL, WNOHANG) == -1);
                    smash1.eraseFromList(command->getID());
                }
                //waitingPID = 0;

            }
        }
    }

        /*************************************************/
    else if (!strcmp(cmd, "bg")) {
        //if arg[2]==NULL find last process that was paused
        //print name of process
        //move process with command number(arg[2]) to bg
        if ((smash1.isJobListEmpty()) || (num_arg > 1)) {
            illegal_cmd = true;
        } else {
            list<job>::iterator iter;
            list<job>::iterator command = smash1.LastjobSuspended();
            switch (num_arg) {
                case 0:  // no args- find last process sent to bg
                    //if (suspended_counter) {  // case there are jobs in the bg
                    // checking if there was a job suspended
                    if (command == smash1.getListEnd()) {
                        // if no job was suspended
                        illegal_cmd = true;
                        return -1;
                    }

                    break;
                case 1: // find the relevant job
                    if (has_only_digits(args[1])) {
                        command = smash1.getJobFromId(atoi(args[1]));
                        if (command == smash1.getListEnd()) {
                            illegal_cmd = true;
                        } else if (command->isSus()) {
                            cout << command->getJobName() << endl;
                        }
                    } else {
                        illegal_cmd = true;
                    }
                    break;

                default:
                    illegal_cmd = true;
                    break;
            }
            if (!illegal_cmd) {
                if (command->isSus()) {
                    string name = command->getJobName();
                    //int pid_ = command->getPID();
                    if (send_sig(command->getPID(), SIGCONT) == 0) {
                        cout << name << endl;
                        command->jobUnsuspended();
                    }
                }
            }
        }


        }


        /*************************************************/
    else if (!strcmp(cmd, "quit")) {
        if (num_arg == 0)// quit
        {
            exit(0);
        } else if (num_arg == 1) {
            if (strcmp(args[1], "kill") == 0)//quit kill
            {
                smash1.quitKill();
                exit(0);
            } else {
                illegal_cmd = true;

            }
        } else {  //num args!= 1,0
            illegal_cmd = true;
        }

    }
        /*************************************************/

    else if (!strcmp(cmd, "cp")) {
        //check there are 2 args
        if (num_arg != 2)
            illegal_cmd = true;
        else if (!strcmp(args[1], args[2])) {
            perror("smash error:> ");
            return (-1);
        } else {     //check for success
            ifstream src(args[1], ios::binary);
            //if (src.rdbuf() == NULL) {
            if (!src.is_open()) {
                perror("smash error:> ");
                return (-1);
            }
            ofstream dst(args[2], ios::binary);
            dst << src.rdbuf();
            if (dst.rdbuf() == NULL) {
                perror("smash error:> ");
                return (-1);
            } else {
                cout << args[1] << " has been copied to " << args[2] << endl;
            }
            //  TODO: check if copied properly
            //   perror("smash error:> ");

        }
    }
        /*************************************************/
    else if (!strcmp(cmd, "diff")) {
        //check there are 2 args
        if (num_arg != 2) {
            illegal_cmd = true;
            perror("smash error:> ");
        }

        fstream file1, file2;

        file1.open(args[1], ios::in | ios::binary | ios::ate);
        file2.open(args[2], ios::in | ios::binary | ios::ate);

        //verify both files were opened right
        if (file1.is_open() && file2.is_open()) {
            if (areFilesEqual(&file1, &file2)) {
                //cout << "Files are equal";
                cout << "0" << endl;
            } else
                //cout << "Files are different";
                cout << "1" << endl;
        } else
            //cout << "The file couldn't be opened properly" << endl;
            perror("smash error:> ");
    }
        /*************************************************/


    else // external command
    {
        ExeExternal(args, cmdString, false);
        return 0;
    }
    if (illegal_cmd == true) {
        printf("smash error: > \"%s\"\n", cmdString);
        return 1;
    }
    return 0;
}

//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char *cmdString, bool bg) {
    int pID;
    // in case of error, the err msg will be as follow
    string err_string("smash error:> \\");
    err_string += args[0];
    err_string += "\" - ";
    switch (pID = fork()) {
        case -1:
            // Add your code here (error)
            // new process wasn't launched properly
            perror("ERROR- fork");
            exit(1);
            break;
        case 0 :
            // Child Process
            setpgrp();
            // Add your code here (execute an external command)
            if (execvp(args[0], args) == -1) {
                perror("exec ERROR");
                exit(1);
            } else {
                exit(0);
            }
            break;
        default:
            //  Father code
            string name = args[0];
            //  if it's not on background - we will wait for it
            if (!bg) {
                //int status;

                waitingPID = pID;
                job FG = job(pID, name, false);
                smash1.setFgJob(FG);
                waitpid(pID, NULL, 0);
                //waitpid(pID, &status, WUNTRACED);

            }
                // job is bg
            else {
                job BG = job(pID, name, false);
                smash1.addJob(BG);
                waitpid(pID, NULL, WNOHANG);

            }

    }
}

//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char *lineSize) {
    //char ExtCmd[MAX_LINE_SIZE + 2];
    //  char *args[MAX_ARG];
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) ||
        (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&"))) {
        // Add your code here (execute a complicated command)

        /*
        your code
        */
    }
    return -1;
}

//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char *lineSize, char *prev_path) {

    char *Command;
    const char *delimiters = " \t\n";
    char *args[MAX_ARG];
    if (lineSize[strlen(lineSize) - 2] == '&') {
        lineSize[strlen(lineSize) - 2] = '\0';
        // Add your code here (execute a in the background)
        Command = std::strtok(lineSize, delimiters);
        if (Command == NULL)
            return 0;
        args[0] = Command;
        for (int i = 1; i < MAX_ARG; i++) {
            args[i] = std::strtok(NULL, delimiters);

        }
        ExeExternal(args, Command, true);
        return 0;

    }
    return -1;
}

bool areFilesEqual(fstream *a, fstream *b) {
    int fileSize1 = sizeOfFile(a);
    int fileSize2 = sizeOfFile(b);

    if (fileSize1 == fileSize2) {
        int BUFFER_SIZE;

        if (fileSize1 > 1024)
            BUFFER_SIZE = 1024;
        else
            BUFFER_SIZE = fileSize1;

        char *file1buffer = new char[BUFFER_SIZE];
        char *file2buffer = new char[BUFFER_SIZE];

        do {
            a->read(file1buffer, BUFFER_SIZE);
            b->read(file2buffer, BUFFER_SIZE);

            if (memcmp(file1buffer, file2buffer, BUFFER_SIZE) != 0) {
                //cout << "Files are not equal, at least one of the byte was different" << endl;

                delete[] file1buffer;
                delete[] file2buffer;
                return false;
            }
        } while (a->good() && b->good());

        delete[] file1buffer;
        delete[] file2buffer;
        return true;
    } else {
        //cout << "Size of Files are not equal" << endl;
        return false;
    }
}

int sizeOfFile(fstream *file) {
    file->seekg(0, ios::end);
    int sizeOfFile = file->tellg();
    file->seekg(0, ios::beg);
    return sizeOfFile;
}
