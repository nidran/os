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

// Reads the random file and create an array of random numbers
void readRandom(string filename, deque<int> &randomNo)
{
    ifstream randoms(filename);
    if (randoms.good()) {
        int current_number = 0;

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

int ofs=0;
int myrandom(int burst, deque<int> &randomNo)
{

    if(ofs==randomNo.size()) ofs=0;
    return (randomNo[ofs++] % burst);
}



void printFrameTable(frame_t *FrameTable, int MAX_FRAMES)
{
    cout << "FT: ";
    for(int i=0; i<MAX_FRAMES; i++)
    {
        if(FrameTable[i].process==NULL)
        {
            cout << "* ";
        }
        else
        {
            cout << FrameTable[i].process->pid << ":" << FrameTable[i].vpage->vPageNo << " ";
        }
    }

    cout << endl;
}

void initialiseFT(frame_t *frameTable, int MAX_FRAMES){

    for (int i = 0; i < MAX_FRAMES; i++) {
        frameTable[i] = *(new frame_t);
        frameTable[i] = i;
    }
}


void printUnmap(Process * curProc, pte_t * p){
    cout<<" UNMAP "<<curProc->pid<<":"<<p->vPageNo<<endl;

}

void updatePTE(pte_t *pte,int f, vma * vma){
    pte->referenced = 0;
    pte->modified = 0;
    pte->fileMapped = vma->file;
    pte->write_protected = vma->write;
    pte->present = 1;
    pte->frameNumber = f;


}
bool isNull(pte_t * frame) {
    if (frame ==NULL)
        return true;
    return false;
}

void printSelected ( bool popt, bool sopt, bool fopt, vector<Process> &Processes,frame_t *FrameTable, int MAX_FRAMES, all overall){
    if(popt){
        for (auto p : Processes) {
            p.printPageTable();
        }
    }

    if (fopt) {
        printFrameTable(FrameTable,MAX_FRAMES);
    }

    if (sopt) {
        for (auto p : Processes) {
            cout << p;
        }
        cout<<"TOTALCOST "<<overall.ic<<" "<<overall.tcs<<" "<<overall.tpe<<" "<<overall.tc<<" "<<sizeof(pte_t)
        <<endl;
    }

}

vma * isUnallocated(int page, Process * curProcess)
{
    if(curProcess->pageTable[page].used==1)
    {
        return &curProcess->vmas.at(curProcess->pageTable[page].vma);
    }
    int i=0;

    for(vma v: curProcess->vmas)
    {
        if(v.start<= page && v.end>=page)
        {
            curProcess->pageTable[page].used=1;
            curProcess->pageTable[page].vma = i;
            return &curProcess->vmas.at(i);
        }

        i++;
    }
    return NULL;
}
void checkModifiedMapped(pte_t *p, all &overall, Process * curP){
    if(p->modified==1 && p->fileMapped==1)
    {
        cout<<" FOUT"<<endl;
        curP->stat.fouts++;
        overall.updateTC(2400);

    }
}


