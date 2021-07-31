//
// Created by Nidhi Ranjan on 06/07/21.
//
#pragma once

#include <iostream>
#include "Process.cpp"

enum trans_state_t {
    TRANS_TO_READY, TRANS_TO_RUN, TRANS_TO_BLOCK, TRANS_TO_PREEMPT
};
using namespace std;

class Event {
public:
    int timestamp;
    Process *pro;
    trans_state_t transition;

    Event(Process *p) {
        this->pro = p;
//        cout<<"Arrival time " <<p->arrivalTime<<endl;
        this->timestamp = p->arrivalTime;

    }

    bool operator>(const Event &e) {
        if (this->timestamp > e.timestamp)
            return true;
        return false;
    }
};