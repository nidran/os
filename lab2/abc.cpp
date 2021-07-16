//
// Created by Nidhi Ranjan on 13/07/21.
//

//
// Created by Nidhi Ranjan on 06/07/21.
//

#include<iostream>
#include<deque>
#include "Process.cpp"

using namespace std;

class Scheduler {
protected:
    deque<Process *> runQ;

public:
    virtual void addProcess(Process *process) = 0;

    virtual Process *getNextProcess() = 0;
    virtual void decay(Process * process){
        if ( process->curState == PREMPT)
            process->dynPri --;
    }
    virtual void reset(Process * process){
        process->dynPri = process->staticPri - 1;
        process->curState = READY;
    }

};

class FCFS : public Scheduler {
public:
    void addProcess(Process *process) {
        runQ.push_back(process);
    }

    Process *getNextProcess() {
        Process *p;
        if (!runQ.empty()) {
            p = runQ.front();
            runQ.erase(runQ.begin());
        } else {
            return NULL;
        }
        return p;
    }
};

class LCFS : public Scheduler {

public:

    void addProcess(Process *p) {
        runQ.push_back(p);
    }

    Process *getNextProcess() {
        Process *p;
        if (!runQ.empty()) {
            p = runQ.back();
            runQ.pop_back();
        } else {
            return NULL;
        }
        return p;
    }

};

class SRTF : public Scheduler {
public:
    int find(Process *process) {
        int idx = 1;
        for (auto i = runQ.begin(); i != runQ.end(); i++) {

            Process *p = *i;
            if (p->totalCPU - p->procState.cpuTotal > process->totalCPU - process->procState.cpuTotal)
                return idx;
            idx++;
        }

        return -1;
    }

    void addProcess(Process *process) {

        int idx = find(process);
//        cout<<"IDX"<<idx;
        if (idx == -1) {
//            cout<<" Insert at end "<<endl;
            runQ.push_back(process);

        } else {
//            cout<<" Insert at index "<<endl;
            auto it = runQ.begin();
            advance(it, idx - 1);
            runQ.insert(it, process);
        }
    }

    Process *getNextProcess() {
        Process *p;
        if (!runQ.empty()) {
            p = runQ.front();
            runQ.pop_front();
        } else {
            return NULL;
        }
        return p;
    }
};

class RoundRobin : public Scheduler {
public:
    void addProcess(Process *process) {

        runQ.push_back(process);
    }

    Process *getNextProcess() {
        Process *p;
        if (!runQ.empty()) {
            p = runQ.front();
            runQ.pop_front();
        } else {
            return NULL;
        }
        return p;
    }
};

class PRIO : public Scheduler {
    vector <deque<Process *> > *activeQ, *expiredQ; // pointers to the queue
    vector <deque<Process *> > aQ, bQ; // actual queue
    int countA, countE;
    int maxPr;

public:
    PRIO(int p){
        maxPr=p;
    }

    // initialize the queues and the pointers
    void initialize() {
        for (int i = 0; i < maxPr; i++) {
            aQ.push_back(deque<Process *>());
            bQ.push_back(deque<Process *>());
        }

        activeQ = &aQ;
        expiredQ = &bQ;
        countA = 0;
        countE = 0;
    }

    void addProcess(Process *process) {
        if (activeQ == NULL)
            initialize();


        if (process->curState == PREMPT) {
            process->dynPri--;
        }

        process->curState = READY;

        if (process->dynPri < 0) {
            process->dynPri = process->staticPri - 1;
            expiredQ->at(process->dynPri).push_back(process);
            countE++;
        } else {
            // cout << "process added" << endl;

            activeQ->at(process->dynPri).push_back(process);
            countA++;
        }
    }

    //  swaps the pointers
    void swap() {
        // cout << "swap called" << endl;
        vector <deque<Process *> > *temp = activeQ;
        activeQ = expiredQ;
        expiredQ = temp;

        countA = countE;
        countE = 0;
    }

    Process *getNextProcess() {
        if (countA == 0)
            swap();

        Process *p = NULL;
        int Qn = -1;

        for (int i = 0; i < activeQ->size(); i++) {
            if (!activeQ->at(i).empty()) {
                Qn = i;
            }
        }

        if (Qn >= 0) {
            p = activeQ->at(Qn).front();
            activeQ->at(Qn).pop_front();
            countA--;
        }

        return p;
    }
};

//  preemptive priority scheduler
class PrePRIO : public Scheduler {
    vector <deque<Process *> > active, expired; // actual array of queues
    vector <deque<Process *> > *activeQ, *expiredQ; // pointers to the queue
    int sizeActive, sizeExpired;
    int maxPr;
public:
    PrePRIO(int p) {
        maxPr = p;
    }
    void setPointers() {
        activeQ = &active;
        expiredQ = &expired;
        sizeActive = 0;
        sizeExpired = 0;
    }
    void initialize() {
        for (int i = 0; i < maxPr; i++) {
            active.push_back(deque<Process *>());
            expired.push_back(deque<Process *>());
        }

    }


    void addProcess(Process *process) {
        if (activeQ == NULL){
            initialize();
            setPointers();
        }
        decay(process);
        if (process->dynPri < 0) {
            reset(process);
            sizeExpired++;
        } else {
            process->curState = READY;
            activeQ->at(process->dynPri).push_back(process);
            sizeActive++;
        }
    }

    void reinitialise() {
        vector <deque<Process *> > *temp = activeQ;
        activeQ = expiredQ;
        expiredQ = temp;
        sizeActive = sizeExpired;
        sizeExpired = 0;
    }
    int findActiveProcess(){
        int idx = 0;
        for (int i = 0; i < activeQ->size(); i++) {
            if (!activeQ->at(i).empty()) {
                idx = i;
                return idx;
            }
        }
        return -1;
    }

    Process *getNextProcess() {
        if (sizeActive == 0)
            reinitialise();
        Process *p = NULL;

        int idx = findActiveProcess();
        if (idx >= 0) {
            p = activeQ->at(idx).front();
            activeQ->at(idx).pop_front();
            sizeActive--;
        }
        return p;
    }
};

