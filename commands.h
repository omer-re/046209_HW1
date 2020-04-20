#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
//#include <time.h>
#include <stdlib.h>
#include <signal.h>
//#include <string.h>

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

#define MAX_LINE_SIZE 80
#define MAX_ARG 20
using namespace std;
#define MAX_HISTORY 50

using namespace std;

class job {
private:
    int id;
    pid_t pid;
    time_t Time;
    time_t susTime; //suspended time

    string jobName;
    bool suspended; //


public:
    static int job_counter;
    static int suspended_counter;

    //Constructor
    job(int jpid, string name, bool sus) : id(job_counter++), pid(jpid), jobName(name), suspended(sus) {

        Time = time(NULL);// get current time--- check?
    }


    job() {
        id = -1;
        jobName = "-";
        suspended = false;
    }


    //gets:
    int getID() {
        return id;
    }

    int getPID() {
        return id;
    }

    bool isSus() {
        return suspended;
    }

    time_t getTime() {
        return Time;
    }

    time_t getSuspendedTime() {
        return susTime;
    }

    string getJobName() {
        return jobName;
    }

    //setters:
    void jobSuspended() {
        susTime = time(NULL);
        suspended = true;
        this->suspended_counter += 1;
    }

    void jpbUnsuspended() {
        suspended = false;
        // TODO Should we zero BG time here or continue it?
        this->suspended_counter -= 1;
    }


};

class smash {
private:
    list<job> jobs;
    pid_t ID;
    vector<string> history;
    int last_processID_on_fg;  //foreground job
    //another val for previous path

public:
    smash()//fg_ = new job(-1, "-", false)
    {

        //fg_ = new job(-1, "-", false);
        //constructor
        ID = getpid();
        jobs.clear();
        history.clear();

        // somthing with previous path

        //define the fg

    }
    int getLastProcessOnFg() const {
        return last_processID_on_fg;
    }

    void setLastProcessOnFg(int lastProcessOnFg) {
        last_processID_on_fg = lastProcessOnFg;
    }

    ~smash() {  //d'tor
        jobs.clear();
        history.clear();
    }
    // smash funcs

    int getSid() { //get ID
        return ID;
    }

    void printHist()   //print the history vector
    {
        if (!history.empty()) //if  history vector is not empty
        {
            for (vector<string>::iterator it = history.begin(); it != history.end(); ++it) {
                cout << *it << endl;

            }
        }
    }

    const list<job>::iterator getListEnd() {
        return jobs.end();  //TODO is this a valid value? if not- needs a return value for last job on list for FG func
    }

    void printJobList() {

        if (!jobs.empty()) //if  job list is not empty
        {
            time_t now = time(NULL);
            for (list<job>::iterator it = jobs.begin(); it != jobs.end(); ++it) {

                int diffTime = now - it->getTime();
                if (it->isSus()) {
                    cout << "[" << it->getID() << "] " << it->getJobName() << "  :  " << it->getPID() << " " << diffTime
                         << " secs  (Stopped)" << endl;
                } else
                    cout << "[" << it->getID() << "] " << it->getJobName() << "  :  " << it->getPID() << " " << diffTime
                         << " secs" << endl;


            }
        }
    }

    void addToHistory(string c) // add command  to History vec
    {
        if (history.size() == MAX_HISTORY) {
            history.erase(history.begin());
        }

        history.push_back(c);
    }

    list<job>::iterator getJobFromId(int id) { //check if the job ID exist in the list and return the job
        for (list<job>::iterator it = jobs.begin(); it != jobs.end(); ++it) {
            int itID = it->getID();
            if (itID == id) {
                return it;
            }

        }
        return jobs.end();  //job not in the list

    }

    bool isInJList(int idj) {
        list<job>::iterator it = getJobFromId(idj);

        if (it == jobs.end())
            return false;
        return true;

    }


    list<job>::iterator LastjobSuspended()//

    {
        int maxtime = -1;
        list<job>::iterator last = jobs.begin();

        for (list<job>::iterator it = jobs.begin(); it != jobs.end(); ++it) {
            if (it->isSus()) {
                int sTime = it->getSuspendedTime();
                if (sTime > maxtime) {
                    last = it;
                    maxtime = sTime;
                }
            }

        }
        return last;
    }

    int LastInBg() //TODO check that this is how "job is in the bg" is defined
    {
        int last = -1;
        for (list<job>::iterator it = jobs.begin(); it != jobs.end(); ++it) {
            // TODO make sure it isn't last on fg!
            if (!it->isSus() && it->getID() != getLastProcessOnFg()) // unsuspended and not the last one n the fg
            {
                last = it->getID();
            }
        }
        return last;
    }

    void eraseFromList(int id) //erase from job list
    {
        for (list<job>::iterator it = jobs.begin(); it != jobs.end(); ++it) {
            if (it->getID() == id)
                it = jobs.erase(it);

        }

    }

};

int ExeComp(char *lineSize);

int BgCmd(char *lineSize, void *jobs, std::string prev_path);

int ExeCmd(void *jobs, char *lineSize, char *cmdString, bool bg, string &prev_path, smash &smash1);

void ExeExternal(char *args[MAX_ARG], char *cmdString, smash &smash1, bool bg);

extern int waitingPID;

#endif