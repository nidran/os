//
// Created by Nidhi Ranjan on 04/07/21.
//

#include<iostream>
#include<fstream>
#include<getopt.h>
#include <deque>
#include<queue>
#include<string>
#include "Helper.cpp"
#include "Process.cpp"
#include "Event.cpp"
#include "Scheduler.cpp"
#include "Des.cpp"

using namespace std;

void simulation(Des *des);

void printRun();

int maxPr = 4;
deque<int> randomNo;
deque <Process> processQ;
enum verbose {
    VERBOSE, NON_VERBOSE
};
bool ioSwitch = false;
Scheduler *scheduler;
int totalIO = 0;
int totalCPU = 0;
int totalCW = 0;
int totalTT = 0;
int cpuBurst = 0;
int lastEveFin = 0;
int quantam = 10000;
verbose ver;
int curTime=0;
int timePrevState = 0;
bool isPremptPri = false;
bool isCreatedOrBlocked ( Process * process);
int findTS(Process * process, Des* des);
string printState(process_state_t v);

void printVerbose(verbose ver, int curTime, Process *process, int timePrevState);

void printVerbose(verbose ver, int curTime, Process *process, int timePrevState, string str);

void printVerbose(verbose ver, int curTime, Process *process, int timePrevState, int cpuBurst);

void printVerbose(verbose ver, int curTime, int ib, Process *process, int timePrevState);

void printVerbose(verbose ver, int curTime, Process *process, int timePrevState, string prev, string cur);

void updateTCU( ){
    totalCPU+=timePrevState;
}
void updateTCW(){
    totalCW += timePrevState;
}
void updateTT(int time){
    totalTT +=time;
}
string algo;

int main(int argc, char **argv) {


    char temp;
    int mode;
    while ((mode = getopt(argc, argv, "vtes:")) != -1) {
        switch (mode) {
            case 'v':
//                cout<<"v";
                ver = VERBOSE;
//                cout<<ver;
                break;
            case 's':
                ver = NON_VERBOSE;
                if (optarg != NULL) {
                    switch (optarg[0]) {
                        case 'F':
                            algo = "FCFS";
                            scheduler = new FCFS();
                            break;
                        case 'L':
                            algo = "LCFS";
                            scheduler = new LCFS();
                            break;
                        case 'S':
                            algo = "SRTF";
                            scheduler = new SRTF();
                            break;
                        case 'R':
                            scheduler = new RoundRobin();
                            quantam = atoi(&optarg[1]);
//                            cout<<quantam<<" ";
                            if (quantam != 10000) {
                                algo = "RR " + to_string(quantam);
                            }

                            break;

                        case 'P':
                            algo = "PRIO";
//                            cout<<algo;
//                            quantam = atoi(&optarg[1]);
//                            maxPr= (atoi(&optarg[3]));

                            sscanf(optarg, "%*c%d:%d", &quantam, &maxPr);
//                            cout<<quantam<<" "<<maxPr;
                            scheduler = new PRIO(maxPr);
                            if (quantam != 10000) {
                                algo = "PRIO " + to_string(quantam);
                            }

//                            cout<<algo;

                            break;
                        case 'E':
                            algo = "PREPRIO";
                            sscanf(optarg, "%*c%d:%d", &quantam, &maxPr);
//                            cout<<quantam<<" "<<maxPr;
                            scheduler = new PrePRIO(maxPr);
                            isPremptPri = true;

                            if (quantam != 10000) {
                                algo = "PREPRIO " + to_string(quantam);
                            }

//                            cout<<algo;
                            break;
                        default:
                            cout << "Error! exiting";
                            return 1;
                    }
                }
                break;
            default:
                break;
        }

    }
//    getBursts("rfile", randomNo);
//    getProcesses("input0", maxPr, processQ, randomNo);
    getBursts(argv[optind + 1], randomNo);
    getProcesses(argv[optind], maxPr, processQ, randomNo);


    Des des;
    for (int i = 0; i < processQ.size(); i++) {
        Event event(&processQ[i]);
        event.transition = TRANS_TO_READY;
        des.putEvent(event);
    }
    simulation(&des);
    printRun();
    return 0;

}

string printState(process_state_t v) {
    string s;
    if (v == 0)
        s = "CREATED";
    else if (v == 1)
        s = "READY";
    else if (v == 2)
        s = "RUNNG";
    else if (v == 3)
        s = "BLOCK";
    else if (v == 4)
        s = "DONE";
    else if (v == 5)
        s = "PREMPT";
    return s;
}

string printTrans(trans_state_t v) {
    string s;
    if (v == 0)
        s = "TRANS_TO_READY";
    else if (v == 1)
        s = "TRANS_TO_RUN";
    else if (v == 2)
        s = "TRANS_TO_BLOCK";
    else if (v == 3)
        s = "TRANS_TO_PREMPT";

    return s;
}


void simulation(Des *des) {
    Process *process;

    Event *evt;
    bool CALL_SCHEDULER = false;
    int ioStart = 0;

    int ioProcessStat = 0;

    Process *CURRENT_RUNNING_PROCESS = NULL;

    while ((evt = des->getEvent()) != NULL) {
        curTime = evt->timestamp;
        process = evt->pro;
        timePrevState = curTime - process->lastStateTs;
        process->lastStateTs = curTime;
        switch (evt->transition) {
            case TRANS_TO_READY: {

                process->curState = READY;
                printVerbose(ver, curTime, process, timePrevState);

                if (process->prevState == BLOCKED) {
                    process->procState.IOtime += timePrevState;
                    process->dynPri = process->staticPri - 1;
                    ioProcessStat--;
                    if (ioProcessStat == 0) {
                        totalIO += curTime - ioStart;

                    }
                } else if (process->prevState == RUNNING) {
                    updateTCU();
                    process->procState.cpuTotal += timePrevState;


                }
                if (isPremptPri ) {
                    if (isCreatedOrBlocked(process) && CURRENT_RUNNING_PROCESS != NULL) {
                        int ts = findTS(CURRENT_RUNNING_PROCESS, des);
                        // dynamic priority is higher
                        if (process->dynPri > CURRENT_RUNNING_PROCESS->dynPri && curTime < ts) {
                            des->deleteEve(CURRENT_RUNNING_PROCESS);
                            CURRENT_RUNNING_PROCESS->prevState = RUNNING;
                            CURRENT_RUNNING_PROCESS->curState = READY;

                            Event e(CURRENT_RUNNING_PROCESS);
                            e.timestamp = curTime;
                            e.transition = TRANS_TO_PREEMPT;

                            des->putEvent(e);
                        }
                    }
                }

                process->prevState = READY;
                scheduler->addProcess(process);
                CALL_SCHEDULER = true;
                break;
            }
            case TRANS_TO_RUN: {
                CURRENT_RUNNING_PROCESS = process;
                process->curState = RUNNING;

                cpuBurst = process->procState.cpuBurstRem > 0 ? process->procState.cpuBurstRem : myrandom(
                        process->cpuBurst, randomNo);
                if (cpuBurst > process->totalCPU - process->procState.cpuTotal) {
                    cpuBurst = process->totalCPU - process->procState.cpuTotal;
                }
                process->updateCPUWait(timePrevState);
                process->procState.cpuBurstRem = cpuBurst;

//                cout<<printState(process->prevState)<<" "<<printState(process->curState)<<endl;
                printVerbose(ver, curTime, process, timePrevState, cpuBurst);
                updateTCW();

//                process->prevState = RUNNING;
                Event e(process);
                if (cpuBurst > quantam) {
                    e.timestamp = curTime + quantam;
                    e.transition = TRANS_TO_PREEMPT;

                } else {
                    e.timestamp = curTime + cpuBurst;
                    e.transition = TRANS_TO_BLOCK;
                }

                des->putEvent(e);
                break;
            }
            case TRANS_TO_BLOCK: {
                CURRENT_RUNNING_PROCESS = NULL;

                updateTCU();
                process->updateCPUtaken();
                process->updateCPUBurstRem(timePrevState);
                // done
                if (process->totalCPU - process->procState.cpuTotal == 0) {

                    process->curState = DONE;

                    process->procState.finTime = curTime;
                    process->procState.turnaround = curTime - process->arrivalTime;


                    updateTT( process->procState.turnaround);
                    printVerbose(ver, curTime, process, timePrevState, "DONE");
                    lastEveFin = curTime;


                } else {
                    process->prevState = RUNNING;
                    process->curState = BLOCKED;
                    if (ioProcessStat == 0)
                        ioStart = curTime;

                    ioProcessStat++;

                    int ib = myrandom(process->ioBurst, randomNo);

//                    cout<<printState(process->prevState)<<" ";
                    printVerbose(ver, curTime, ib, process, timePrevState);


                    process->prevState = BLOCKED;
                    Event e(process);
                    e.timestamp = curTime + ib;
                    e.transition = TRANS_TO_READY;
                    des->putEvent(e);
                }
//
                CALL_SCHEDULER = true;
                break;

            }
            case TRANS_TO_PREEMPT: {

                process->updateCPUBurstRem(timePrevState);
                process->procState.cpuTotal += timePrevState;
                process->curState = PREMPT;
                printVerbose(ver, curTime, process, timePrevState, "RUNNG", "READY");
                CURRENT_RUNNING_PROCESS = NULL;

                updateTCU();
                scheduler->addProcess(process);
                CALL_SCHEDULER = true;
                break;
            }
            
        }

        des->removeEvent();


        if (CALL_SCHEDULER) {
            if (des->getNextEventTime() == curTime) {
                continue;
            }

            CALL_SCHEDULER = false;

            if (CURRENT_RUNNING_PROCESS == NULL) {
                CURRENT_RUNNING_PROCESS = scheduler->getNextProcess();

                if (CURRENT_RUNNING_PROCESS == NULL) {
                    continue;
                }

                CURRENT_RUNNING_PROCESS->curState = READY;
                Event e(CURRENT_RUNNING_PROCESS);
                e.timestamp = curTime;
                e.transition = TRANS_TO_RUN;
                des->putEvent(e);
            }
        }

    }
}


void printRun() {
    int size = processQ.size();
    cout << algo << endl;
    for (int i = 0; i < size; i++) {
        cout << processQ.at(i);
    }

    double cpuUtil = ((double) totalCPU / lastEveFin) * 100;
    double ioUtil = ((double) totalIO / lastEveFin) * 100;
    double avgTT = ((double) totalTT / processQ.size());
    double avgCW = ((double) totalCW / processQ.size());
    double throughput = (processQ.size() * 100.0) / lastEveFin;
    cout << "SUM:" << " "
         << lastEveFin << " "
         << setprecision(2) << fixed << cpuUtil << " "
         << setprecision(2) << ioUtil << " "
         << setprecision(2) << avgTT << " "
         << setprecision(2) << avgCW << " "
         << setprecision(3) << throughput << endl;


}

void printVerbose(verbose ver, int curTime, Process *process, int timePrevState) {
    if (ver == 0) {
        cout << curTime << " "
             << process->processID << " "
             << timePrevState << ": "
             << printState(process->prevState) << " -> "
             << printState(process->curState) << endl;

    }
}

void printVerbose(verbose ver, int curTime, Process *process, int timePrevState, int cpuBurst) {
    if (ver == 0) {
        cout << curTime << " "
             << process->processID << " "
             << timePrevState << ": "
             << printState(process->prevState) << " -> "
             << printState(process->curState) << " "
             << "cb=" << cpuBurst << " "
             << "rem=" << process->totalCPU - process->procState.cpuTotal << " "
             << "prio=" << process->dynPri << endl;

    }
}

void printVerbose(verbose ver, int curTime, Process *process, int timePrevState, string str) {
    if (ver == 0)
        cout << curTime << " "
             << process->processID << " "
             << timePrevState << ": "
             << printState(process->curState) << endl;

}

void printVerbose(verbose ver, int curTime, int ib, Process *process, int timePrevState) {
    if (ver == 0) {
        cout << curTime << " "
             << process->processID << " "
             << timePrevState << ": "
             << printState(process->prevState) << " -> "
             << printState(process->curState) << " "
             << "ib=" << ib << " "
             << "rem=" << process->totalCPU - process->procState.cpuTotal << endl;

    }
}

void printVerbose(verbose ver, int curTime, Process *process, int timePrevState, string prev, string cur) {
    if (ver == 0) {
        cout << curTime << " "
             << process->processID << " "
             << timePrevState << ": "
             << prev << " -> "
             << cur << " "
             << "cb=" << process->procState.cpuBurstRem << " "
             << "rem=" << process->totalCPU - process->procState.cpuTotal << " "
             << "prio=" << process->dynPri << endl;
    }
}

bool isCreatedOrBlocked ( Process * process){
return process->prevState == CREATED || process->prevState == BLOCKED;
}

int findTS(Process * process, Des * des){
    int ts = -1;
    for (auto i = des->eventQ.begin(); i != des->eventQ.end(); i++) {
        Event e = *i;
        if (e.pro == process) {
            ts = e.timestamp;
        }
    }

    return ts;
}