//
// Created by Nidhi Ranjan on 24/07/21.
//

#include <iostream>
#include <vector>
#include "pte.cpp"
#pragma once
const int MAX_VPAGES = 64;
using namespace std;

class all {
public:
    unsigned long tpe;
    unsigned long tcs;
    unsigned long tc;
    unsigned long ic;

    all(){
        tpe= 0;
        tcs =0;
        ic =0;
        tc =0;
    }
public :
    void updateTC(int tc){
        this->tc+=tc;
    }
    void init(){
        tpe= 0;
        tcs =0;
        ic =0;
        tc =0;

    }
};
class vma
{
public:
    int start;
    int end;
    int write;
    int file;
    vma(){

    }

    vma(int svp, int evp, int wp, int fm)
    {
        start = svp;
        end = evp;
        write = wp;
        file = fm;
    }
};
class stats {
public :
    unsigned long maps;
    unsigned long unmaps;
    unsigned long ins;
    unsigned long outs;
    unsigned long fins;
    unsigned long fouts;
    unsigned long zeros;
    unsigned long segv;
    unsigned long segprot;
};
class Process
{
public:
    int pid;
    vector<vma> vmas;
    pte_t pageTable[MAX_VPAGES];
    stats stat;


    Process(int pid, vector<vma> v)
    {
        this->pid = pid;
        this->vmas = v;
        int i;
        for(i=0; i<MAX_VPAGES; i++)
        {
            pageTable[i] = *(new pte_t);
            pageTable[i].vPageNo = i;
        }

        stat.unmaps = 0;
        stat.maps = 0;
        stat.ins = 0;
        stat.outs = 0;
        stat.fins = 0;
        stat.fouts = 0;
        stat.zeros = 0;
        stat.segv = 0;
        stat.segprot = 0;
    }

    void printPageTable()
    {
        cout << "PT[" << pid << "]: ";
        int i;
        for(i = 0; i<MAX_VPAGES; i++)
        {
            if (pageTable[i].present == 0 && pageTable[i].pagedout == 1)
            {
                printf("# ");
            }
            else if (pageTable[i].present == 0 && pageTable[i].pagedout == 0)
            {
                printf("* ");
            }
            else
            {
                cout<<i <<':';
                string os;
                os = (pageTable[i].referenced == 1) ? ("R") : ("-");
                cout<<os;
                os = (pageTable[i].modified == 1) ? ("M") : ("-");
                cout<<os;
                os = (pageTable[i].pagedout == 1) ? ("S ") : ("- ");
                cout<<os;

            }


    }
        cout << endl;
    }
    friend ostream& operator<<(ostream& os, const Process& process);
};
ostream& operator<<(ostream& os, const Process & p){
    os<<"PROC["<<p.pid<<"]: "<<"U="<<p.stat.unmaps<<" M="<<p.stat.maps<<" I="<<p.stat.ins<<" O="<<p.stat.outs<<" FI="<<p.stat.fins<<" FO="<<p.stat.fouts
    <<" Z="<<p.stat.zeros<<" SV="<<p.stat.segv<<" SP="<<p.stat.segprot<<endl;
    return os;
}

class frame_t
{
public:
    Process *process;
    pte_t *vpage;
    int frameNo;
    unsigned long age;
    int tau;


    frame_t()
    {
        process = NULL;
        vpage = NULL;
        age = 0;
        tau = 0;
    }

    void reset(Process *process, pte_t *vpage)
    {
        this->process = process;
        this->vpage = vpage;
    }
    void operator = (const int &i ) {
        this->process = NULL;
        this->vpage = NULL;
        this->frameNo = i;

    }
};

class command
{
public:
    char ins;
    int page;
};