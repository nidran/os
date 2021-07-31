
#include <iostream>
#include <vector>
#include <deque>
#include "helper.cpp"
#include "process.cpp"
#pragma once
using namespace std;

class Pager {
protected:
    int idx;
public:
    virtual frame_t *select_victim_frame(frame_t *FrameTable, int MAX_FRAMES, all overall, deque<int> &randomNo) = 0;
    void updateIdx( int frame){
        idx = (idx + 1) % frame;

    }
};

class FIFO : public Pager {

public:
    FIFO() {
        idx = 0;
    }

    frame_t *select_victim_frame(frame_t *FrameTable, int MAX_FRAMES, all overall, deque<int> &randomNo) {
        frame_t *f = &FrameTable[idx];
        updateIdx(MAX_FRAMES);

        return f;
    }
};

class Random : public Pager {
public:

    Random() {
        idx=0;

    }
    frame_t *select_victim_frame(frame_t *FrameTable, int MAX_FRAMES, all overall, deque<int> &randomNo) {
        int victim = myrandom(MAX_FRAMES, randomNo);
        frame_t *f = new frame_t;
        f = &FrameTable[victim];
        return f;
    }
};

class Clock : public Pager {

    bool notFound = true;
    bool updated = true;
public:
    Clock() {
        idx = 0;
    }

    frame_t *select_victim_frame(frame_t *FrameTable, int MAX_FRAMES, all overall, deque<int> &randomNo) {

        while(FrameTable[idx].vpage->referenced==1)
        {
                FrameTable[idx].vpage->referenced=0;
                updateIdx(MAX_FRAMES);
            }

        frame_t *f = &FrameTable[idx];
        idx = (f->frameNo + 1) % MAX_FRAMES;
        return f;

    }
};

class NRU : public Pager {
    int prevResetIns;
    bool reset;
    frame_t *NRUData[4];
    int curFrameClass ;

public:
    NRU() {
        idx = 0;
        prevResetIns = 0;

        for (int i = 0; i < 4; i++) {
            NRUData[i] = nullptr;
        }
    }
    void init(int ic){
        reset = false;
        for (int i = 0; i < 4; i++) {
            NRUData[i] = nullptr;
        }
        if (ic - prevResetIns >= 50)
        {
            reset = true;
            prevResetIns = ic;
        }
    }

    frame_t *select_victim_frame(frame_t *FrameTable, int MAX_FRAMES, all overall, deque<int> &randomNo) {
        init(overall.ic);
        for (int i = 0; i < MAX_FRAMES; i++) {
            curFrameClass = (int) FrameTable[idx].vpage->modified + (2 * (int) FrameTable[idx].vpage->referenced)  ;
            if (NRUData[curFrameClass] == nullptr) {
                NRUData[curFrameClass] = &FrameTable[idx];
            }

            if (reset) {
                FrameTable[idx].vpage->referenced = 0;
            } else if (curFrameClass == 0) {
                break;
            }

            updateIdx(MAX_FRAMES);
        }

        for (int i = 0; i < 4; i++) {
            if (NRUData[i] != nullptr) {
                idx = (NRUData[i]->frameNo + 1) % MAX_FRAMES;
                return NRUData[i];

            }
        }
        return NULL;
    }
};

class Aging : public Pager {

public:
    Aging() {
        idx = 0;
    }

    frame_t *select_victim_frame(frame_t *FrameTable, int MAX_FRAMES, all overall, deque<int> &randomNo) {
        frame_t *victim = &FrameTable[idx];
        frame_t * curFrame;
        for (int i = 0; i < MAX_FRAMES; i++) {
            curFrame = &FrameTable[idx];
            curFrame->age >>=1;
            if (curFrame->vpage->referenced == 1) {
               curFrame->age = curFrame->age | ((unsigned long)1 << 31);
               curFrame->vpage->referenced = 0;
            }

            if (curFrame->age < victim->age) {
                victim = curFrame;
            }

            updateIdx(MAX_FRAMES);
        }

        idx = (victim->frameNo + 1) % MAX_FRAMES;
        return victim;
    }
};


class WorkingSet : public Pager {
public:
    int time = 0;
    WorkingSet() {
        idx = 0;
    }

    frame_t *select_victim_frame(frame_t *FrameTable, int MAX_FRAMES, all overall, deque<int> &randomNo) {
        frame_t *curFrame;
        frame_t *current = &FrameTable[idx];
        frame_t *victim = nullptr;

        int minTime = 0;
        for (int i = 0; i < MAX_FRAMES; i++) {
            curFrame = &FrameTable[idx];

            if (curFrame->vpage->referenced == 1) {
                curFrame->vpage->referenced = 0;
                curFrame->tau = overall.ic;

            } else if (overall.ic - curFrame->tau >= 50) {
                victim = curFrame;
                break;
            } else if (overall.ic - curFrame->tau > minTime) {
                victim = curFrame;
                minTime = overall.ic - curFrame->tau;
            }

            updateIdx(MAX_FRAMES);
        }

        if (victim == nullptr)
            victim = current;

        idx = (victim->frameNo + 1) % MAX_FRAMES;
        return victim;
    }
};
