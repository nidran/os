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
deque <int> randomNo;
deque<Process> processQ;
enum verbose {VERBOSE, NON_VERBOSE} ;
bool ioSwitch = false;
Scheduler *scheduler;
int totalIO=0;
int totalCPU=0;
int totalCW=0;
int totalTT=0;
int cpuBurst=0;
int lastEveFin=0;
verbose ver;
string printState ( process_state_t v);



int main(int argc , char **argv) {


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
                if(optarg!=NULL) {
                    switch (optarg[0]) {
                        case 'F':
                            cout<<"FCFS"<<endl;
                            scheduler = new FCFS();
                            break;
                    }
                }
                break;
            default:
                break;
        }

    }
    getBursts("rfile", randomNo);
    getProcesses (  "input0", maxPr, processQ, randomNo);


    Des des;
    for(int i=0; i<processQ.size();i++) {
        Event event(&processQ[i]);
        event.transition = TRANS_TO_READY;
        des.putEvent(event);
    }
    simulation(&des);
    printRun();
    return 0;

}
string printState ( process_state_t v){
    string s;
    if (v == 0 )
        s= "CREATED";
    else if (v == 1)
        s= "READY";
    else if (v == 2)
        s= "RUNNG";
    else if (v == 3)
        s= "BLOCK";
    else if (v == 4)
        s= "DONE";
    else if (v == 5)
        s= "PREMPT";
    return s;
}
string printTrans ( trans_state_t v){
    string s;
    if (v == 0 )
        s= "TRANS_TO_READY";
    else if (v == 1)
        s= "TRANS_TO_RUN";
    else if (v == 2)
        s= "TRANS_TO_BLOCK";
    else if (v == 3)
        s= "TRANS_TO_PREMPT";

    return s;
}

void simulation(Des *des) {
    int k =0;
    Process *process;
    int curTime;
    int timePrevState=0;
    int ioProcesses = 0;
    Event *evt;
    bool CALL_SCHEDULER = false;
    int ioStart = 0;

    Process *CURRENT_RUNNING_PROCESS = NULL;

    while ((evt = des->getEvent()) != NULL) {
//        cout<<"Event in consideratixon "<<evt->pro->processID<<" "<<printState(evt->pro->curState)<<endl;
        process = evt->pro;
        curTime = evt->timestamp;
//        cout<<"timestamp "<<evt->timestamp<<"curTime"<<" "<<curTime<<endl;
        timePrevState = curTime - process->lastStateTs;
//        cout<<curTime<<" "<<timePrevState;
        process->lastStateTs = curTime;
        cout<<printTrans(evt->transition)<<endl;
        switch (evt->transition) {

            case TRANS_TO_READY: {
//                cout<<evt->pro->processID<<" ";
                process->curState = READY;
                    cout << curTime << " " <<
                    process->processID<< " " <<
                    timePrevState << ": " << printState(process->prevState)
                    << " -> " <<  printState(process->curState) << endl;

                if (process->prevState == BLOCKED) {
                    process->dynPri = process->staticPri - 1;
                    process->procState.IOtime += timePrevState; // time process was idle
//                    ioSwitch = false;
                    ioProcesses--;
                    if(ioProcesses == 0)
                    {
                        totalIO += curTime - ioStart ;
                    }

                } else if (process->prevState == RUNNING) {
                    process->procState.cpuTotal += timePrevState;
                    totalCPU += timePrevState;
                }

                process->prevState = READY;
                scheduler->addProcess(process);
                CALL_SCHEDULER = true;
                break;
            }
            case TRANS_TO_RUN: {
//                cout<<"trans to run"<<endl;
                process->curState = RUNNING;
                CURRENT_RUNNING_PROCESS = process;
                if ( process->procState.cpuBurstRem >0 ){
                    cpuBurst = process->procState.cpuBurstRem;

                }
                else {
                    cpuBurst = myrandom(process->cpuBurst, randomNo);
                    if(cpuBurst > process->totalCPU - process->procState.cpuTotal)
                    {
                        cpuBurst = process->totalCPU - process->procState.cpuTotal;
                    }

                }
                process->procState.cpuWait += timePrevState;
                totalCW += timePrevState;
                process->procState.cpuBurstRem = cpuBurst;

                if(ver == 1)
                    cout << curTime << " " << process->processID<< " " << timePrevState << ": " << printState(process->prevState )<< " -> " << printState(process->curState ) <<" cb="<< cpuBurst << " rem="<<process->totalCPU- process->procState.cpuTotal  <<" prio="<<process->dynPri<<endl;

                process->prevState = RUNNING;
                Event e( process);
                e.timestamp = curTime+cpuBurst;
                e.transition = TRANS_TO_BLOCK;
                des->putEvent(e);
                CALL_SCHEDULER = false;
                break;
            }
            case TRANS_TO_BLOCK :{
//                cout<<"trans to block"<<endl;
                process->procState.cpuTotal += process->procState.cpuBurstRem;
                totalCPU += timePrevState;
                process->procState.cpuBurstRem -= timePrevState;
                if(process->procState.cpuTotal - process->totalCPU == 0)
                {
                    process->curState = DONE;
                    process->prevState = DONE;
                    process->procState.finTime = curTime;
                    process->procState.turnaround= curTime - process->arrivalTime;
                    lastEveFin  = curTime;
                    totalTT += process->procState.turnaround;


                    if(ver == 1)
                        cout << curTime << " " << process->processID<< " " << timePrevState << ": " << printState(process->curState) << " -> " << "DONE" << endl;

                }
                else
                {
                    process->curState = BLOCKED;
                    if (ioProcesses == 0){ioStart = curTime;}
                    ioProcesses++;
                    int ib = myrandom(process->ioBurst, randomNo);
                    if(ver == 1)
                        cout << curTime << " " << process->processID<< " " << timePrevState << ": " << printState(process->prevState) << " -> " << printState(process->curState) <<" ib="<< ib <<" rem="<< process->totalCPU-process->procState.cpuTotal<< endl;

                    process->prevState = BLOCKED;

                    Event curE(process);
                    curE.timestamp = curTime+ ib;
                    des->putEvent(curE);



                }
                CALL_SCHEDULER = true;
                break;
            }

            case TRANS_TO_PREEMPT:  {
//                cout<<"trans to preempt"<<endl;
                process->procState.cpuBurstRem -= timePrevState;
                process->procState.cpuTotal += timePrevState;
                if(ver == 1)
                    cout << curTime << " " << process->processID<< " " << timePrevState << ": " << printState(process->curState) << " -> " << "READY" << endl;

                process->curState = PREMPT;
                process->prevState = PREMPT;
                CURRENT_RUNNING_PROCESS = NULL;
                totalCPU += timePrevState;
                scheduler->addProcess(process);
                CALL_SCHEDULER = true;
                break;
            }

            default :
                break;

        }

        des->removeEvent();
        k++;
        if(CALL_SCHEDULER)
        {
//             cout << "\n here "<<k<<endl;
            if(curTime == des->getNextEventTime() ) {
//                cout<<" curtime == nextTime()";
                continue;
            }
            CALL_SCHEDULER = false;
            if(CURRENT_RUNNING_PROCESS == NULL){
                CURRENT_RUNNING_PROCESS = scheduler->getNextProcess();
//                cout<<"Process ID "<<CURRENT_RUNNING_PROCESS->processID;
                if (CURRENT_RUNNING_PROCESS == NULL){
//                    cout<<"cur running process is null";
                    continue;
                }


//                cout<<"Call scheduler"<<endl;
                Event evt (CURRENT_RUNNING_PROCESS);
                evt.timestamp = curTime;
                evt.transition = TRANS_TO_RUN;
                evt.pro->curState = BLOCKED;
//                cout<<"Put Event ";
                des->putEvent(evt);
            }
//
        }

        }
    }


void printRun() {
    int size = processQ.size();
    for (int i = 0; i < size; i++) {
        cout << processQ.at(i);
    }

    double cpuUtil = ((double) totalCPU / lastEveFin) * 100;
    double ioUtil = ((double) totalIO / lastEveFin) * 100;
    double avgTT = ((double) totalTT / processQ.size());
    double avgCW = ((double) totalCW / processQ.size());
    double throughput = (processQ.size() * 100.0) / lastEveFin;
    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", lastEveFin, cpuUtil, ioUtil, avgTT, avgCW, throughput);


}

