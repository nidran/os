//
// Created by Nidhi Ranjan on 06/07/21.
//

#include <iostream>
#include <ostream>
#include<string>
//#include<cstring.h>
#include <fstream>
#include "Process.cpp"
using namespace std;
static int offset = 0;
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
//    cout<<randomNo.size();
//    cout<<"exit getBursts()"<<endl;
    }


int myrandom(int burst, deque<int> &randomNo)
{

    if( offset == randomNo.size())
        offset =0;
    return 1 + (randomNo.at(offset++) % burst);
}

void getProcesses ( string filename, int pri, deque<Process> &processQ, deque<int> &randomNo){
//    cout<<"hey";
    ifstream file(filename);
    if (file.good()) {
        int pid =0;
        string line;
        while(file>>line)
        {
//            cout<<line;
            int AT, TC, CB, IO;
            AT = stoi(line);
            file>>line;
            TC = stoi(line);
            file>>line;
            CB = stoi(line);
            file>>line;
            IO = stoi(line);
//
//             cout << AT << " " << TC << " " << CB << " " << IO << endl;
//
            Process p(pid,AT,TC,CB,IO);
//            cout<<pri;
            p.staticPri = myrandom(pri, randomNo);
//            cout<<p.staticPri<<"  ";
            p.dynPri = p.staticPri-1;
////            p. = AT;
            processQ.push_back(p);
//            // cout << "process created" << endl;
//            cout<<"PID"<<pid;
            pid++;
        }
//
        file.close();
    }
//    cout<<"exit getProcesses()"<<endl;

}