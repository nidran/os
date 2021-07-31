//
// Created by Nidhi Ranjan on 06/07/21.
//
#pragma once
#include <iomanip>
#include<iostream>
using namespace std;
enum process_state_t {CREATED,READY,RUNNING,BLOCKED, DONE, PREMPT};

class ProcessState {
public:
    int cpuWait =0; // waiting in ready state
    int cpuTotal=0; // cpu time
    int finTime =0; // time when process finished
    int IOtime=0; // block state
    int response=0; // Response time
    int turnaround=0; // Turnaround time
    int cpuBurstRem = 0;


};
class Process {
public:
    int processID;
    int arrivalTime;
    int totalCPU;
    int cpuBurst;
    int ioBurst;
    int staticPri;
    int dynPri;
    process_state_t curState = CREATED;
    process_state_t prevState = CREATED;
    int lastStateTs;
    ProcessState procState;

    Process(int id, int at, int tcp, int cpb, int iob)
    {
        this->processID = id;
        this->cpuBurst = cpb;
        this->totalCPU = tcp;
        this->ioBurst = iob;
        this->arrivalTime = at;
        this->lastStateTs = at;

    }


    void operator = (const Process &p ) {
        this->processID = p.processID;
        this->cpuBurst = p.cpuBurst;
        this->totalCPU = p.totalCPU;
        this->ioBurst = p.ioBurst;
        this->arrivalTime = p.arrivalTime;
        this->staticPri = p.staticPri;
        this->dynPri = p.dynPri;
    }


    void updateDynPr() {
        // if quantum expires
        dynPri-= 1;
        if (dynPri == -1) {
            dynPri = staticPri - 1;

        }
    }
    void updateCPUWait(int time){
        procState.cpuWait += time;
    }
    void updateCPUBurstRem(int time){
        procState.cpuBurstRem-= time;
    }
    void updateCPUtaken(){
        procState.cpuTotal += procState.cpuBurstRem;
    }
    friend ostream& operator<<(ostream& os, const Process& process);

};


ostream& operator<<(ostream& os, const Process & p)
{

    os << setfill('0') << setw(4)<< p.processID <<": "
    <<setfill(' ') <<setw(4)<< p.arrivalTime
    <<" "<<setfill(' ') << setw(4)<<
    p.totalCPU<<" "<<setfill(' ') << setw(4)<<
    p.cpuBurst<<" "<<setfill(' ') << setw(4)<<
    p.ioBurst <<" "<<setfill(' ') << setw(1)<<
    p.staticPri<<" | "<<setfill(' ') << setw(5)<<
    p.procState.finTime<<" "<<setfill(' ') << setw(5)<<
    p.procState.turnaround<<" "<<setfill(' ') << setw(5)<<
    p.procState.IOtime<<" "<<setfill(' ') << setw(5)<<p.procState.cpuWait<<endl;
    return os;

}

class frame_t
{
public:
    Process *process;
    pte_t *vpage;
    int frameNo;
    uint32_t age;
    int tau;

public:
    frame_t()
    {
        process = NULL;
        vpage = NULL;
        age = 0;
        tau = 0;
    }

    void setFrame(Process *process, PTE *vpage)
    {
        this->process = process;
        this->vpage = vpage;
    }
};
