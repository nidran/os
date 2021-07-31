#include<iostream>
#include <list>
#include <iterator>
#include<getopt.h>
#include<fstream>
#include<string.h>
#include<stdio.h>
#include<vector>
#include<queue>
#include "pte.cpp"
#include "data.cpp"
#include "data.cpp"
#include "helper.cpp"
#define FOR(a, b, c) for (int(a) = (b); (a) < (c); ++(a))
using namespace std;

bool sopt;
bool fopt;
bool popt;
bool oopt;
all overall;
deque<int> randomNo;
int total_rand;
vector<Process> processes;
frame_t * FrameTable;
deque<frame_t *> unallocated;
int MAX_FRAMES;
Pager *pager;
Process *curProcess;
vector<command> commands;
frame_t * getFrame();
void get_next_instruction(command e);
void processFrame(frame_t * frame);


int main(int argc, char *argv[])
{
    int opt;
    while((opt = getopt (argc,argv,"f:a:o:")) != -1)
    {
        switch(opt)
        {
            case 'f':
                MAX_FRAMES = stoi(optarg);
                break;

            case 'o':   if(optarg!=NULL)
                {
                    string choices = optarg;
                    for(int i=0; i<choices.size();i++)
                    {
                        switch(choices[i])
                        {
                            case 'S':
                                sopt = true;
                                break;
                            case 'F':
                                fopt = true;
                                break;
                            case 'P':
                                popt = true;
                                break;
                            case 'O':
                                oopt = true;
                                break;
                        }
                    }
                }
                break;

            case 'a':
                if(optarg!=NULL)
                {
                    overall.init();
                    switch(optarg[0])
                    {
                        case 'f':
                            pager = new FIFO();
                            break;
                        case 'r':
                            pager = new Random();
                            break;
                        case 'c':
                            pager = new Clock();
                            break;
                        case 'e':
                            pager = new NRU();
                            break;
                        case 'a':
                            pager = new Aging();
                            break;
                        case 'w':
                            pager = new WorkingSet();
                            break;
                    }
                }
                break;
            default:

                break;
        }
    }

    string fileName = argv[optind];
    string randFileName = argv[optind+1];
    readRandom(randFileName, randomNo);
    FrameTable = new frame_t[MAX_FRAMES];
    initialiseFT(FrameTable, MAX_FRAMES);

    for ( int i = 0; i < MAX_FRAMES; i++){
        unallocated.push_back(&FrameTable[i]);
    }

    ifstream file;
    file.open(fileName);
    string line;
    bool start = true;
    int curP =0;
    int numProcess;
    while(getline(file,line))
    {
        if(line.at(0)!='#')
        {
            char *c = new char[1000];
            strcpy(c, &line[0]);
            if(start)
            {
                sscanf(c,"%d",&numProcess);
                start = false;
                continue;
            }
            else if(curP < numProcess)
            {
                int numVMA;
                sscanf(c,"%d",&numVMA);

                vector<vma> v;
                for(int j=0; j<numVMA; j++)
                {
                    do
                    {
                        getline(file,line);

                    }
                    while (line.at(0)=='#');
                    char d[1000];
                    strcpy(d, &line[0]);
                    vma tmp;
                    sscanf(d,"%d%d%d%d", &tmp.start, &tmp.end, &tmp.write, &tmp.file);
                    v.push_back(tmp);
                }
                Process process(curP, v);
                processes.push_back(process);
                curP++;
            }
            else
            {
                char cm;
                int p;
                sscanf(c,"%c%d",&cm,&p);
                command e;
                e.ins = cm;
                e.page = p;
                get_next_instruction(e);
            }
        }
    }

    printSelected( popt,sopt , fopt, processes, FrameTable, MAX_FRAMES, overall);


    return 0;
}


frame_t * getFrame()
{
    // Check if some frame is free
    if(!unallocated.empty())
    {
        frame_t *f = unallocated.front();
        unallocated.pop_front();
        return f;
    }

    frame_t *f =  pager->
            select_victim_frame(FrameTable, MAX_FRAMES, overall, randomNo);
    return f;
}
void get_next_instruction(command e)
{
    if(oopt){
        printf("%lu: ==> %c %d\n", overall.ic,  e.ins, e.page);
    }

    overall.ic++;

    if(e.ins=='c')
    {
        curProcess = &processes.at(e.page);
        overall.tcs++;
        overall.updateTC(130);

    }
    else if(e.ins == 'e')
    {
        cout<<"EXIT current process "<<curProcess->pid<<endl;

        FOR(i, 0,MAX_VPAGES )
        {
            pte_t *p = &curProcess->pageTable[i];
            if(p->present==1)
            {
                p->present=0;
                curProcess->stat.unmaps++;
                overall.updateTC(400);
                oopt ?  printUnmap(curProcess, p) : void();
                FrameTable[p->frameNumber].reset(nullptr, nullptr);
                unallocated.push_back(&FrameTable[p->frameNumber]);
                checkModifiedMapped(p, overall, curProcess);
            }
            p->pagedout=0;
        }


        curProcess = NULL;
        overall.tpe++;
        overall.updateTC(1250);

    }
    else
    {
        pte_t *pte = &curProcess->pageTable[e.page];
        if(pte->present==0)
        {
            vma *vma;
            vma = isUnallocated(e.page, curProcess);


            if (vma == nullptr)
            {
                overall.updateTC(340);
                curProcess->stat.segv++;
                if(oopt)
                    cout<<" SEGV\n";
            }
            else
            {
                frame_t *frame = getFrame();
                processFrame( frame);
                frame->reset(curProcess, pte);

                updatePTE(pte, frame->frameNo, vma);
                if(pte->pagedout==1)
                {
                    overall.updateTC(3100);
                    curProcess->stat.ins++;
                    if(oopt)
                        cout<<" IN\n";
                }
                else if(pte->fileMapped==1)
                {
                    curProcess->stat.fins++;
                    overall.updateTC(2800);
                    if(oopt)
                        cout<<" FIN\n";
                }
                else
                {
                    curProcess->stat.zeros++;
                    overall.updateTC(140);
                    if(oopt)
                        cout<<" ZERO\n";
                }

                if(oopt)
                {
                    cout<<" MAP "<< frame->frameNo<<endl;
                }
                overall.updateTC(300);
                curProcess->stat.maps++;

            }
        }

        if(e.ins=='r')
        {
            pte->referenced=1;
            overall.updateTC(1);
        }
        else if(e.ins=='w')
        {
            if(pte->write_protected==0)
            {
                pte->referenced=1;
                pte->modified=1;
            }
            else
            {
                overall.updateTC(420);
                curProcess->stat.segprot++;
                if(oopt)
                    cout<<" SEGPROT\n";
                pte->referenced=1;
            }
            overall.updateTC(1);
        }

    }
}

void processFrame(frame_t * frame){
    if(frame->vpage!=NULL)
    {
        oopt ?  printUnmap(frame->process, frame->vpage) : void();

        frame->process->stat.unmaps++;
        frame->vpage->present=0;

        overall.updateTC(400);

        if(frame->vpage->modified==1)
        {
            if(frame->vpage->fileMapped==1)
            {
                overall.updateTC(2400);
                frame->process->stat.fouts++;
                if(oopt)
                    cout<<" FOUT\n";
            }
            else
            {
                frame->process->stat.outs++;
                overall.updateTC(2700);
                frame->vpage->pagedout = 1;
                if(oopt)
                    cout<<" OUT\n";

            }
        }
    }
    frame->age = 0;
    frame->tau = overall.ic;
}





