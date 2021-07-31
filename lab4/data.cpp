//
// Created by Nidhi Ranjan on 24/07/21.
//

#include <iostream>
#include <vector>
#include "pte.cpp"
#pragma once


class ioJob {
    int startTime;
    int trackNo;

    ioJob():

};

class all {
    int totalTime;
    int totMovement;
    int avgTurnaround;
    int avgWaitTime;
    int maxWaitTime;

    all():
    {
        this->avgTurnaround = 0;
        this->totalTime=0;
        this->totMovement =0;
        this->avgWaitTime=0;
        this->maxWaitTime=0;

    }
};