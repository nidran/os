//
// Created by Nidhi Ranjan on 24/07/21.
//

#include <iostream>
#include <vector>
#include <deque>
#include "Process.cpp"
#pragma once
using namespace std;

class Pager{
public:
    virtual frame_t* select_victim_frame( int idx, int frames, frame_t * ft) = 0;
};

class FIFO : public Pager {
    int idx = 0;
public:

    frame_t * select_victim_frame( int idx, int frames , frame_t * ft) {

        cout<<" in fifo";
        frame_t *f  = &ft[idx];
        this->idx = (this->idx + 1) % frames;
        return f;

    }

};




