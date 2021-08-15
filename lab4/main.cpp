#include<iostream>
#include<getopt.h>
#include<fstream>
#include<stdio.h>
#include<deque>
#include<unistd.h>
#include<string.h>
#include "data.cpp"
#include "ioSched.cpp"
#include "helper.cpp"

deque <jobDetails> jb;
deque <ioJob> IOJobsDone;
curState cur;
all info;
ioSched *scheduler;
ioJob *curReq;

void simulate();
void JobOver(ioJob r);
void printRun();
using namespace std;

int main(int argc, char *argv[]) {
    int option;
    while ((option = getopt(argc, argv, "vqfs:")) != -1) {
        switch (option) {
            case 's':
                if (optarg != NULL) {
                    switch (optarg[0]) {
                        case 'i':
                            scheduler = new FIFO();
                            break;
                        case 'j':
                            scheduler = new SSTF();
                            break;
                        case 's':
                            scheduler = new Look();
                            break;
                        case 'c':
                            scheduler = new CLook();
                            break;
                        case 'f':
                            scheduler = new FLook();
                            break;
                    }
                }
                break;
            case 'f':
                break;
            default:
                break;
        }
    }

    string fileName = argv[optind];
    readFile(fileName, jb);
    curReq = NULL;
    simulate();
}

void JobOver(ioJob r) {
    int k = findIdx(IOJobsDone, r);
    if (k != -1) {
        deque<ioJob>::iterator it = IOJobsDone.begin();
        advance(it, k - 1);
        IOJobsDone.insert(it, r);
    } else {
        IOJobsDone.push_back(r);
    }
}

void printRun() {
    for (int i = 0; i < IOJobsDone.size(); i++) {
        cout << IOJobsDone.at(i);
    }
}

void simulate() {

    while (true) {
        if (curReq == NULL && scheduler->isEmpty() && jb.empty()) {
            break;
        } else {
            if (!jb.empty() && jb.front().time == cur.curTime) {
                scheduler->addIOJob(new ioJob(info.reqNo, jb.front().track, cur.curTime));
                jb.pop_front();
                info.reqNo++;
            }

            if (curReq != NULL && curReq->reqTrack == cur.curTrack) {
                curReq->endTime = cur.curTime;
                info.totTurnAround += cur.curTime - curReq->arrivalTime;
                ioJob done;
                done.updateIoJob(curReq);
                JobOver(done);
                curReq = NULL;
                scheduler->removeIOJob();

            }

            if (curReq == NULL && !scheduler->isEmpty()) {
                curReq = scheduler->strategy(cur.curTime, jb, cur);
                if (curReq != NULL) {
                    curReq->startTime = cur.curTime;
                    info.totWaitTime += cur.curTime - curReq->arrivalTime;
                    info.maxWaitTime =
                            cur.curTime - curReq->arrivalTime > info.maxWaitTime ? cur.curTime - curReq->arrivalTime
                                                                                 : info.maxWaitTime;
                    if (curReq->reqTrack == cur.curTrack)
                        cur.curTime--;
                } else {
                    cur.curTrack--;
                    info.upMovement();
                    info.downWaitTime();
                    if (cur.curDir == -1 && cur.curTrack == 0)
                        cur.curDir = 1;
                }

            }
            if (curReq != NULL && curReq->reqTrack != cur.curTrack) {
                info.upMovement();
                cur.updateTrack();
            }
            cur.curTime++;
        }
    }

    printRun();
    cur.curTime--;
    printf("SUM: %d %d %.2lf %.2lf %d\n", cur.curTime, info.totMovement, double(info.totTurnAround) / info.reqNo,
           double(info.totWaitTime) / info.reqNo, info.maxWaitTime);
}

