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
    void decay(Process * process){
        if (process->curState == PREMPT) {
            process->dynPri--;
        }

    }
    void reset(Process * process){
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
    vector <deque<Process *> > active, expired; // actual array of queues
    vector <deque<Process *> > *activeQ, *expiredQ; // pointers to the queue
    int sizeA, sizeE;
    int maxPr;
public:
    PRIO(int p) {
        maxPr = p;
    }
    void reintialise() {
        vector <deque<Process *> > *temp = activeQ;
        activeQ = expiredQ;
        expiredQ = temp;
        sizeA = sizeE;
        sizeE = 0;
    }
    void setPointers() {
        activeQ = &active;
        expiredQ = &expired;
        sizeA = 0;
        sizeE = 0;
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
            expiredQ->at(process->dynPri).push_back(process);
            sizeE++;
        } else {
            process->curState = READY;
            activeQ->at(process->dynPri).push_back(process);
            sizeA++;
        }
    }
    int findNonEmpty(){
        int pos = -1;
        for (auto i = 0; i < activeQ->size(); i++) {
            if (!activeQ->at(i).empty()) {
                pos = i;

            }
        }

        return pos;
    }


    Process *getNextProcess() {
        if (sizeA == 0)
            reintialise();

        Process *curr = NULL;
        int idx = findNonEmpty();

        if (idx >= 0) {
            curr = activeQ->at(idx).front();
            activeQ->at(idx).pop_front();
            sizeA--;
        }

        return curr;
    }
};

class PrePRIO : public Scheduler {
    vector <deque<Process *> > active, expired; // actual array of queues
    vector <deque<Process *> > *activeQ, *expiredQ; // pointers to the queue
    int sizeA, sizeE;
    int maxPr;
public:
    PrePRIO(int p) {
        maxPr = p;
    }
    void reintialise() {
        vector <deque<Process *> > *temp = activeQ;
        activeQ = expiredQ;
        expiredQ = temp;
        sizeA = sizeE;
        sizeE = 0;
    }
    void setPointers() {
        activeQ = &active;
        expiredQ = &expired;
        sizeA = 0;
        sizeE = 0;
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
            expiredQ->at(process->dynPri).push_back(process);
            sizeE++;
        } else {
            process->curState = READY;
            activeQ->at(process->dynPri).push_back(process);
            sizeA++;
        }
    }
    int findNonEmpty(){
        int pos = -1;
        for (auto i = 0; i < activeQ->size(); i++) {
            if (!activeQ->at(i).empty()) {
                pos = i;

            }
        }

        return pos;
    }


    Process *getNextProcess() {
        if (sizeA == 0)
            reintialise();

        Process *curr = NULL;
        int idx = findNonEmpty();

        if (idx >= 0) {
            curr = activeQ->at(idx).front();
            activeQ->at(idx).pop_front();
            sizeA--;
        }

        return curr;
    }
};

