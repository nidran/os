//
// Created by Nidhi Ranjan on 26/07/21.
//

#include <iostream>
#include <ostream>
#include<string>
#include <fstream>
#include "data.cpp"

#pragma once
using namespace std;

int findIdx(deque <ioJob> &IOJobsDone, ioJob r) {
    int idx = 1;
    for (auto i = IOJobsDone.begin(); i != IOJobsDone.end(); i++) {

        if (i->reqNo > r.reqNo) {

            return idx;
        }
        idx++;
    }
    return -1;
}

void readFile(string fileName, deque <jobDetails> &jb) {
    int time;
    int track;
    ifstream file;
    file.open(fileName);
    string line;
    while (getline(file, line)) {
        if (line.at(0) != '#') {
            char ch[1000];
            strcpy(ch, &line[0]);
            sscanf(ch, "%d %d", &time, &track);
            jobDetails j(time, track);
            jb.push_back(j);
        }
    }


}
