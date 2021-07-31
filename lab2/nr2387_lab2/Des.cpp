//
// Created by Nidhi Ranjan on 06/07/21.
//
;
#pragma once

#include<iostream>
#include<deque>
#include "Event.cpp"

using namespace std;


class Des {
public:
    deque <Event> eventQ;

    int find(int time) {
        int idx = 0;
        for (auto i = eventQ.begin(); i != eventQ.end(); i++) {
            idx++;
            if (time < i->timestamp) {
                return idx;
            }
        }
        return -1;
    }

    int findEveIdx(Process *p) {
        int idx = 1;
        for (auto i = eventQ.begin(); i != eventQ.end(); i++) {

            Event e = *i;
            if (e.pro == p) {
                return idx;
            }
            idx++;
        }
        return -1;
    }


    void putEvent(Event event) {
        int idx = find(event.timestamp);
        if (idx == -1) {
            eventQ.push_back(event);

        } else {
            auto it = eventQ.begin();
            advance(it, idx - 1);
            eventQ.insert(it, event);
        }

    }

    void deleteEve(Process *p) {
        int idx = findEveIdx(p);
        if (idx != -1) {
            auto it = eventQ.begin();
            advance(it, idx - 1);
            eventQ.erase(it);
        }
    }
    Event *getEvent() {
        return eventQ.empty() ? NULL : &eventQ.front();
    }

    int getNextEventTime() {
        return eventQ.empty() ? -1 : eventQ.front().timestamp;
    }

    void removeEvent() {
        eventQ.pop_front();
    }




};


