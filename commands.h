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

    string jobName;
    bool suspended; //
    time_t Time;
    time_t susTime; //suspended time



public:


    //Constructor
    job(int jpid, string name, bool sus) : id(job_counter++), pid(jpid), jobName(name), suspended(sus) {

        Time = time(NULL);// get current time--- check?
    }

    job(int jpid, int jid, string name, time_t jtime) : id(jid), pid(jpid), jobName(name), Time(jtime) {
        suspended = false;
    }


    job() {
        pid = -1;
        jobName = "-";
        suspended = false;
    }

    ~job() {
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

    void jobUnsuspended() {
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

    list<job>::iterator getJobFromPID(int _pid) { //check if the job PID exist in the list and return the job
        for (list<job>::iterator it = jobs.begin(); it != jobs.end(); ++it) {
            int itPID = it->getPID();
            if (itPID == _pid) {
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
        if (result == -1) {
            perror("error:");
            exit(1);
        }

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


    int LastInBgOLD() {
        int last = -1;
        for (list<job>::iterator it = jobs.begin(); it != jobs.end(); ++it) {
            if (!it->isSus() && it->getID() != getLastProcessOnFg()) // unsuspended and not the last one n the fg
            {
                last = it->getID();
            }
        }
        return last;
    }

    list<job>::iterator LastInBg() {
        list<job>::iterator last = jobs.begin();
        time_t lastbgTime = last->getTime();

        for (list<job>::iterator it = jobs.begin(); it != jobs.end(); ++it) {
            time_t k = it->getTime();
            if (k > lastbgTime) {
                last = it;
                lastbgTime = k;

            }

        }
        //cout << "last is   " << last->getJobName() << endl;
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
            int status;

            int k = kill(jpid, SIGTERM);
            if (k == -1) {
                perror("smash error: >");
                return;
            }
            string jname = it->getJobName();
            int jid = it->getID();
            sleep(5);
            if (waitpid(jpid, &status, WNOHANG) == jpid) {

                cout << "[" << jid << "] " << jname << " - Sending SIGTERM...Done." << endl;  //

                continue;
            }
                //process didnt die after 5 sec
            else {
                int kk = kill(jpid, SIGKILL);
                if (kk == -1) {
                    perror("smash error: >");
                    return;
                }
                cout << "[" << jid << "] " << jname << " - Sending SIGTERM...(5 sec passed) Sending SIGKILL...Done."
                     << endl;
            }
        }

    }

    bool isJobListEmpty() {
        return jobs.empty();
    }


    list<job> &GetJobs() {
        return jobs;
    }

};

int ExeComp(char *lineSize);

int BgCmd(char *lineSize, char *prev_path);

int ExeCmd(char *lineSize, char *cmdString, bool bg, char *prev_path);

void ExeExternal(char *args[MAX_ARG], char *cmdString, bool bg);

#endif