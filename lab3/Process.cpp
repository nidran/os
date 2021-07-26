//
// Created by Nidhi Ranjan on 24/07/21.
//

#include <iostream>
#include <vector>
#include "pte.cpp"
#pragma once
const int MAX_VPAGES = 64;
using namespace std;
typedef struct {
    char ins_type;
    int page;
} command;



class pstats {
public :
    int unmaps;
    int maps;
    int segv;
    int segprot;
    int fins;
    int fouts;
    int outs;
    int ins;
    int zeros;

    pstats(){
        this->fins = 0;
        this->fouts = 0;
        this->unmaps = 0;
        this->maps = 0;
        this->segprot = 0;
        this->segv = 0;
        this->outs = 0;
        this->ins = 0;
        this->zeros = 0;
    }
};
class vma {
public :
    int start;
    int end;
    int read;
    int write;


    vma(int s, int e, int r, int w){
        this->start = s;
        this->end = e;
        this->read = r;
        this->write = w;
    }
};

class Process {
public :
    int proc_id;
    vector<vma> process;
    pstats *stats;
    pte_t pageTable[MAX_VPAGES];


    Process (){

    };
    Process  (int i, vector<vma> v){
        this->proc_id = i;
        this->process = v;
        pstats();

        // populate page table for process
        for ( int i =0 ; i< MAX_VPAGES; i++){
            pageTable[i] = *(new pte_t);
            pageTable[i].setvPageNo(i);
        }
    }
    void printPageTable()
    {
        cout << "PT[" << proc_id << "]: ";
        int i;
        for(i = 0; i<MAX_VPAGES; i++)
        {
            if(pageTable[i].getPresentBit()==1)
            {
                cout << i << ":";

                if(pageTable[i].getReferencedBit()==1)
                    cout << "R";
                else
                    cout << "-";

                if(pageTable[i].getModifiedBit()==1)
                    cout << "M";
                else
                    cout << "-";

                if(pageTable[i].getPagedOutBit()==1)
                    cout << "S ";
                else
                    cout << "- ";
            }
            else
            {
                if(pageTable[i].getPagedOutBit()==1)
                {
                    cout << "# ";
                }
                else
                {
                    cout << "* ";
                }
            }
        }
        cout << endl;
    }

    void printSummary()
    {
//        printf("PROC[%d]: U=%lu M=%lu I=%lu O=%lu FI=%lu FO=%lu Z=%lu SV=%lu SP=%lu\n", pid,
//               unmaps, maps, ins, outs, fins, fouts, zeros, segv, segprot);
    }


};

class frame_t {
public :
int frame;
int proc_id;
//int vpage;
int age;
int tau;
Process * process;
pte_t *vpage;


     frame_t(){

 }
    void reset(Process * process, pte_t *vpage){
     this->process = process;
     this->vpage = vpage;
 }

 void reset(string str){
     this->process = nullptr;
     this->vpage = 0;
 }
    void reset(int frame_no){
        this->process = nullptr;
        this->vpage = 0;
    }
};




