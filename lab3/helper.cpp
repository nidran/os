//
// Created by Nidhi Ranjan on 26/07/21.
//

#include <iostream>
#include <ostream>
#include<string>
//#include<cstring.h>
#include <fstream>
#include "Process.cpp"
using namespace std;
void getBursts(string filename, deque<int> &randomNo) {
    ifstream randoms(filename);
    if (randoms.good()) {
        int current_number = 0;
//        bool size = false;
        while (randoms >> current_number) {
            randomNo.push_back(current_number);
        }
        randoms.close();
    } else {
        cout << "Error!";
        exit(0);
    }
    randomNo.pop_front();

}
