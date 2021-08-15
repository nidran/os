//
// Created by Nidhi Ranjan on 24/07/21.
//
#include <iomanip>
#include <iostream>

#include <vector>

using namespace std;
#pragma once

class jobDetails {
public:
    jobDetails(int time, int track) {
        this->time = time;
        this->track = track;
    }

    int time;
    int track;
};

class ioJob {
public:
    int reqNo;
    int arrivalTime;
    int startTime;
    int endTime;
    int reqTrack;

    ioJob() {

    }

    ioJob(int reqNo, int track, int timestamp) {
        this->reqNo = reqNo;
        this->reqTrack = track;
        this->arrivalTime = timestamp;
    }

    ioJob(int a, int t) {
        this->arrivalTime = a;
        this->reqTrack = t;

    }

    ioJob(int reqNo, int track, int timestamp, int arrivalTime, int endTime, int startTime) {
        this->reqNo = reqNo;
        this->reqTrack = track;
        this->arrivalTime = timestamp;
        this->arrivalTime = arrivalTime;
        this->endTime = endTime;
        this->startTime = startTime;
    }

    void updateIoJob(ioJob *job) {
        this->reqNo = job->reqNo;
        this->reqTrack = job->reqTrack;
        this->arrivalTime = job->arrivalTime;
        this->endTime = job->endTime;
        this->startTime = job->startTime;

    }

    friend ostream &operator<<(ostream &os, const ioJob &req);
};

ostream &operator<<(ostream &os, const ioJob &p) {
    os << setfill(' ') << setw(5) << p.reqNo << ": "
       << setfill(' ') << setw(5) << p.arrivalTime << " "
       << setfill(' ') << setw(5) << p.startTime << " "
       << setfill(' ') << setw(5) << p.endTime << endl;
    return os;
}


class all {
public :
    int totalTime;
    int totMovement;
    int totTurnAround;
    int totWaitTime;
    int maxWaitTime;
    int reqNo;

    all() {
        this->totTurnAround = 0;
        this->totalTime = 0;
        this->totMovement = 0;
        this->totWaitTime = 0;
        this->maxWaitTime = 0;
        this->reqNo = 0;


    }

    void upMovement() {
        this->totMovement++;
    }

    void downWaitTime() {
        this->totWaitTime--;
    }

};


class curState {
public :
    int curDir;
    int curTrack;
    int curTime;

    curState() {
        curTime = 0;
        curTrack = 0;
        curDir = 1;
    }

    void updateTrack() {
        this->curTrack = this->curDir == 1 ? this->curTrack + 1 : this->curTrack - 1;

    }

};