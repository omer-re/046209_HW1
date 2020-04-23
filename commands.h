#ifndef _COMMANDS_H
#define _COMMANDS_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
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
#define MAX_HISTORY 50
using namespace std;

extern int waitingPID;
extern int suspended_counter;
extern int job_counter;

class job {
private:
    int id;
    pid_t pid;
    time_t Time;
    time_t susTime; //suspended time

    string jobName;
    bool suspended; //


public:


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
        return pid;
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
        suspended_counter++;
    }

    void jpbUnsuspended() {
        suspended = false;
        suspended_counter--;
    }


};

class smash {
private:
    list<job> jobs;
    pid_t ID;
    vector<string> history;
    int last_processID_on_fg;  //foreground job
    job fg_;  //foreground job
    //another val for previous path


public:
    smash() {
        fg_ = job();
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
        list<job>::iterator last = jobs.end();

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

    void jobStatus(pid_t jpid) {
        int status;
        int result = waitpid(jpid, &status, WNOHANG);
        if (result == jpid) {
            eraseJobFromList(jpid);
        }
    }

    void updateJobs() { // for jobs cmd- check if job finished- and erase from list
        for (list<job>::iterator it = jobs.begin(); it != jobs.end(); ++it) {
            jobStatus(it->getPID());

        }

    }


    void eraseJobFromList(int pid) //erase from job list
    {
        for (list<job>::iterator it = jobs.begin(); it != jobs.end(); ++it) {
            if (it->getPID() == pid)
                it = jobs.erase(it);

        }

    }

    void addJob(job j) {
        jobs.push_back(j);
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

    job fgJob() {
        return fg_;
    }

    void setFgJob(job j) {
        fg_ = j;
    }

    void removeFromFG() {
        fg_ = job();
    }


    void quitKill()//for quit kill cmd
    {

        for (list<job>::iterator it = jobs.begin(); it != jobs.end(); ++it) {
            pid_t jpid = it->getPID();
            int k = kill(jpid, SIGTERM);
            if (k == -1) {
                perror("smash error: >");
                return;
            }
            string jname = it->getJobName();
            int jid = it->getID();
            cout << "[" << jid << "] " << jname << " - Sending SIGTERM...";

            sleep(5);

            if (!kill(jpid, 0)) {
                cout << "Done." << endl;  //
                continue;
            }
            //process didnt die after 5 sec
            k = kill(jpid, SIGKILL);
            if (k == -1) {
                perror("smash error: >");
                return;
            }
            cout << "(5 sec passed) Sending SIGKILL�. Done" << endl;

            it = jobs.erase(it);

        }

    }
};

int ExeComp(char *lineSize);

int BgCmd(char *lineSize, void *jobs, std::string prev_path);

int ExeCmd(void *jobs, char *lineSize, char *cmdString, bool bg, char *prev_path);

void ExeExternal(char *args[MAX_ARG], char *cmdString, bool bg);

#endif