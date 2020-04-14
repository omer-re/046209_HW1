//		commands.c
//********************************************
#include "commands.h"

using namespace std;

int bsdChecksumFromFile(FILE *fp);

bool areFilesEqual(fstream *, fstream *);

int sizeOfFile(fstream *);

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command std::string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(void *jobs, char *lineSize, char *cmdString, std::string &prev_path) {
    char *cmd;
    char *args[MAX_ARG];
    char pwd[MAX_LINE_SIZE];


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
        if (num_arg == 1) {
            if (getcwd(pwd, sizeof(pwd)) == NULL)
                perror("smash error: >");

            if (!strcmp(args[1], "-")) {
                chdir(prev_path.c_str());
                cout << prev_path;
                prev_path = pwd;
            } else if (chdir(args[1]) == -1) {
                printf("smash error: > %s - path not found\n", args[1]);
            }
        } else
            illegal_cmd = true;

    }

        /*************************************************/
    else if (!strcmp(cmd, "pwd")) {
        if (num_arg == 0) {
            if (getcwd(pwd, sizeof(pwd)) == NULL)
                perror("smash error: >");
            else
                printf("%s\n", pwd);
        } else
            illegal_cmd = true;
    }

        /*************************************************/
    else if (!strcmp(cmd, "history")) {

    }
        /*************************************************/
    else if (!strcmp(cmd, "jobs")) {

    }
        /*************************************************/
    else if (!strcmp(cmd, "kill")) {

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

    }
        /*************************************************/
    else if (!strcmp(cmd, "bg")) {
        //if arg[2]==NULL find last process that was paused
        //print name of process
        //move process with command number(arg[2]) to bg
        bool have_stopped = false;
        list<job>::iterator iter, command = Jobs.LastjobSuspended();
        switch (num_arg) {

            case 0:  // handle the last job suspended, move to bg
                if (command._stopped) {  // TODO make sure such field exists
                    // checking if there was a job suspended
                    have_stopped = true;
                }
                if (!have_stopped && !Jobs.size())  // TODO check such method exists
                    // in case jobs list isn't empty but none was suspended

                    break;

            case 1: // find the relevant job and move it
                command = jobs.find(atoi(args[1]));
                break;

            default:
                illegal_cmd = true;
                break;
        }
        if (!illegal_cmd) {
            if (Jobs.LastjobSuspended() != NULL) {  // some job suspended

                if (send_sig(command._PID, SIGCONT) == 0) {
                    command->second._stopped = false;   // TODO fit to our functions and data set
                    command->second._bgTime = time(0);      // TODO fit to our functions and data set
                    printf("%s\n", command->second._name.c_str()); // TODO fit to our functions and data set
                }


            } else // job was already running
            {
                printf("smash error: > job [%d] is already running in the background\n", command->first);
            }


        }


    }
        /*************************************************/
    else if (!strcmp(cmd, "quit")) {

    }
        /*************************************************/

    else if (!strcmp(cmd, "cp"))  //TODO do all of the errors has to be followed with "illegal command"?
    {
        //check there are 2 args
        if (num_arg != 2)
            illegal_cmd = true;
        else if (!strcmp(args[1], args[2])) {
            perror("smash error:> ");
            return (-1);
        } else {     //check for success
            ifstream src(args[1], ios::binary);
            if (src.rdbuf() == NULL) {
                perror("smash error:> ");
                return (-1);
                break;
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
        ExeExternal(args, cmdString);
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
void ExeExternal(char *args[MAX_ARG], char *cmdString) {
    int pID;
    switch (pID = fork()) {
        case -1:
            // Add your code here (error)

            /*
            your code
            */
            break;
        case 0 :
            // Child Process
            setpgrp();

            // Add your code here (execute an external command)

            /*
            your code
            */
            break;


        default:
            // Add your code here

            /*
            your code
            */
            break;

    }
}

//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char *lineSize) {
    char ExtCmd[MAX_LINE_SIZE + 2];
    char *args[MAX_ARG];
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
int BgCmd(char *lineSize, void *jobs, std::string prev_path) {

    char *Command;
    char *delimiters = " \t\n";
    char *args[MAX_ARG];
    if (lineSize[strlen(lineSize) - 2] == '&') {
        lineSize[strlen(lineSize) - 2] = '\0';
        // Add your code here (execute a in the background)

        /*
        your code
        */

    }
    return -1;
}

// checksum function
int bsdChecksumFromFile(FILE *fp) /* The file handle for input data */
{
    int checksum = 0;             /* The checksum mod 2^16. */

    for (int ch = getc(fp); ch != EOF; ch = getc(fp)) {
        checksum = (checksum >> 1) + ((checksum & 1) << 15);
        checksum += ch;
        checksum &= 0xffff;       /* Keep it within bounds. */
    }
    return checksum;
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
