//
// Created by Nidhi Ranjan on 31/07/21.
//

#include <iostream>
#include <vector>
#include <deque>
#include "data.cpp"
#pragma once
using namespace std;

class ioSched {
public:
    deque<ioJob *> IOQueue;
    deque<ioJob *> *active;
    deque<ioJob *> *add;
    int curIndex;
    virtual ioJob *strategy(int timestamp, deque <jobDetails> &ioJobs, curState &curState) = 0;
    virtual void addIOJob(ioJob *r) {
        IOQueue.push_back(r);
    };
    virtual bool isEmpty() {
        return IOQueue.empty();
    }
    virtual void removeIOJob() {
        IOQueue.pop_front();
    }

};

class FIFO : public ioSched {
public:
    FIFO() {

    }

    ioJob *strategy(int timestamp, deque <jobDetails> &ioJobs, curState &cs) {
        ioJob *r = IOQueue.front();
        cs.curDir = cs.curTrack > r->reqTrack ? -1 : 1;
        return r;
    }
};

class SSTF : public ioSched {
public:
    SSTF() {

    }
    int findLoc(deque <jobDetails> &ioJobs, curState &cs){
        bool unset = true;
        int pos = 0;
        int minPos = 0;
        int minDiff = -1;
        int dist;
        for (deque<ioJob *>::iterator it = IOQueue.begin(); it != IOQueue.end(); ++it) {
            pos++;
            ioJob *req = *it;
            dist = abs(cs.curTrack - req->reqTrack);
            if ( unset ) {
                minPos = pos - 1;
                minDiff = dist;
                unset = false;
            }
            else if (minDiff > dist && !unset) {
                minPos = pos - 1;
                minDiff = dist;
            }
        }
        return minPos;
    }

    ioJob *strategy(int timestamp, deque <jobDetails> &ioJobs, curState &cs) {
        curIndex = findLoc( ioJobs,  cs);
        ioJob *r = IOQueue.at(curIndex);
        cs.curDir = cs.curTrack > r->reqTrack ? -1 :1;
        return r;
    }

    void removeIOJob() {
        auto it = IOQueue.begin();
        advance(it, curIndex);
        IOQueue.erase(it);
    }
};

class Look : public ioSched {

public:
    Look() {
        curIndex = 0;
    }

    ioJob *strategy(int timestamp, deque <jobDetails> &ioJobs, curState &cs) {
        int pos = 0;
        int minPos = 0;
        int minDiff = -1;
        int dist;
        bool isMore = false;
        bool unset = true;
        for (auto it = IOQueue.begin(); it != IOQueue.end(); ++it) {
            pos++;
            ioJob *req = *it;
            dist = req->reqTrack - cs.curTrack;
            if (cs.curDir == 1 && dist >= 0) {
                isMore = true;
                if (unset) {
                    minPos = pos - 1;
                    minDiff = abs(dist);
                    unset = false;
                }
                if (minDiff > abs(dist) && !unset) {
                    minPos = pos - 1;
                    minDiff = abs(dist);
                }
            } else if (cs.curDir == -1 && dist <= 0) {
                isMore = true;
                if (unset) {
                    minPos = pos - 1;
                    minDiff = abs(dist);
                    unset = false;
                }
                if (minDiff > abs(dist) && !unset) {
                    minPos = pos - 1;
                    minDiff = abs(dist);
                }
            }
        }
        if (!isMore) {
            cs.curDir = cs.curDir == -1 ? 1 :-1;
            return this->strategy(timestamp, ioJobs, cs);
        }
        curIndex = minPos;
        ioJob *r = IOQueue.at(minPos);
        return r;
    }

    void removeIOJob() {
        auto it = IOQueue.begin();
        advance(it, curIndex);
        IOQueue.erase(it);
    }

};

class CLook : public ioSched {
public:
    CLook() {

    }

    ioJob *strategy(int timestamp, deque <jobDetails> &ioJobs, curState &cs) {
        int pos = 0;
        int minPos = 0;
        int minDiff = -1;
        int dist;
        bool isMore = false;
        bool unset = true;
        int track = cs.curTrack;
        int trackPos = -1;
        for (auto it = IOQueue.begin(); it != IOQueue.end(); ++it) {
            pos++;
            ioJob *req = *it;
            dist = req->reqTrack - cs.curTrack;
            if (dist >= 0) {
                isMore = true;
                if (unset) {
                    minPos = pos - 1;
                    minDiff = abs(dist);
                    unset = false;
                }
                if (minDiff > abs(dist) && !unset) {
                    minPos = pos - 1;
                    minDiff = abs(dist);
                }
            }
            if (track > req->reqTrack) {
                track = req->reqTrack;
                trackPos = pos - 1;
            }
        }
        if (!isMore) {
            cs.curDir = -1;
            curIndex = trackPos;
        } else {
            cs.curDir = 1;
            curIndex = minPos;
        }
        ioJob *r = IOQueue.at(curIndex);
        return r;
    }

    void removeIOJob() {
        auto it = IOQueue.begin();
        advance(it, curIndex);
        IOQueue.erase(it);
    }


};

class FLook : public ioSched {
    deque<ioJob *> tempQueue;
    int pos;
    int minPos;
    int minDiff;
    int dist;
    bool isMore;
public:
    FLook() {
        active = &IOQueue;
        add = &tempQueue;
    }
    void swap(deque<ioJob *> *active, deque<ioJob *> *add){
        deque < ioJob * > *temp = active;
        active = add;
        add = temp;
    }

    bool isEmpty() {
        if (add->empty() && active->empty()){
            return true;
        }
        if (!add->empty() && active->empty()){
            deque < ioJob * > *temp;
            temp = active;
            active = add;
            add = temp;
        }
        return false;

    }

    ioJob *strategy(int timestamp, deque <jobDetails> &ioJobs, curState &cs) {
        if (active->empty()) {
           swap(active, add);
        }
         pos = 0;
         minPos = 0;
         minDiff = -1;
         isMore = false;
         bool unset = true;

        for (auto it = active->begin(); it != active->end(); ++it) {
            pos++;
            ioJob *req = *it;
            dist = req->reqTrack - cs.curTrack;

            if (cs.curDir == 1 && dist >= 0) {
                isMore = true;
                if (unset) {
                    minPos = pos - 1;
                    minDiff = abs(dist);
                    unset = false;
                }

                if (minDiff > abs(dist) && !unset) {
                    minPos = pos - 1;
                    minDiff = abs(dist);
                }
            } else if ( dist <= 0 && cs.curDir == -1 ) {
                isMore = true;
                if (unset) {
                    minPos = pos - 1;
                    minDiff = abs(dist);
                    unset = false;
                }

                if (minDiff > abs(dist) && !unset) {
                    minPos = pos - 1;
                    minDiff = abs(dist);
                }
            }
        }
        if (!isMore) {
            cs.curDir = cs.curDir == -1 ? 1:-1;
            return this->strategy(timestamp, ioJobs, cs);
        }
        curIndex = minPos;
        ioJob *r = active->at(minPos);
        return r;
    }

    void addIOJob(ioJob *r) {
        add->push_back(r);
        if (active->empty()) {
            swap(active, add);
        }
    }

    void removeIOJob() {
        deque<ioJob *>::iterator it = active->begin();
        advance(it, curIndex);
        active->erase(it);
    }

};

