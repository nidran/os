
#include "Process.cpp"
#include "Pager.cpp"
#include <vector>
#include<iostream>
#include<getopt.h>
#include<fstream>

using namespace std;
int MAX_FRAMES;
Pager *THE_PAGER;
vector <Process> processes;
vector <command> commands;
pte_t page_table[MAX_VPAGES];
frame_t *frame_table;
int tcs = 0;
int tpe = 0;
int ic = 0;
int tc = 0;
bool oopt = true;
bool sopt = false;
bool fopt = false;
bool popt = false;

void readFile(string file_name);
void getBursts(string file_name, deque<int> & randomNo);
void simulate(frame_t *frame_table);
void initialiseFrames(frame_t * frame_table);
deque<int> randomNo;
deque<frame_t *> available;
frame_t * getFrame(Pager *pager );

vma * isAvailable(int page, Process *p)
{
//    cout<<"here";
    if(p->pageTable[page].getVMAset()==1)
    {
//        cout<<"is avail";
        return &p->process.at(p->pageTable[page].getVMA());
    }
    int i=0;

    for(vma v: p->process)
    {
//        cout<<"is avail 2";
        if(v.start <= page && v.end >=page)
        {
            p->pageTable[page].setVMAset();
            p->pageTable[page].setvPageNo( i);
            cout<<"i"<<i<<endl;
            return &p->process.at(i);
        }

        i++;
    }
    cout<<"null";
    return NULL;
}

int main(int argc, char *argv[]) {
    int option;

    while ((option = getopt(argc, argv, "f:a:o:")) != -1) {
        switch (option) {
            case 'o':
                break;

            case 'f':
                MAX_FRAMES = stoi(optarg);
                break;

            case 'a':
                if (optarg != NULL) {
                    switch (optarg[0]) {
                        case 'f':
                            THE_PAGER = new FIFO();
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
    getBursts(randFileName, randomNo);
    readFile(fileName);


    frame_table = new frame_t[MAX_FRAMES];
    initialiseFrames(frame_table);

    simulate(frame_table);
    return 0;
}


int ofs = 0;

// returns the next random number
//int myrandom(int burst)
//{
//    if(ofs==total_rand) ofs=0;
//    return 1 + (rand_num[ofs++] % burst);
//}
// Reads the file and creates process objects
void readFile(string file_name) {
    ifstream file;
    file.open(file_name);
    string line;
    if (file.is_open()) {
        bool start= true;
        int totProcessC;
        int curProcessC = 0;
        while (getline(file, line)) {

            if (line.at(0) != '#') {
                char ch_str[line.size() + 1];
                strcpy(ch_str, &line[0]);
                if (start) {
                    sscanf(ch_str, "%d", &totProcessC);
                    start = false;
                    continue;
                } else if (curProcessC < totProcessC) {
                    int totVmaC;
                    sscanf(ch_str, "%d", &totVmaC);

                    vector <vma> vm;
                    for (int j = 0; j < totVmaC; j++) {
                        do {
                            getline(file, line);


                        } while (line.at(0) == '#');
                        char ch_str1[line.size() + 1];
                        strcpy(ch_str1, &line[0]);
                        int svp, evp, wp, fm;
                        sscanf(ch_str1, "%d%d%d%d", &svp, &evp, &wp, &fm);
                        vma v(svp, evp, wp, fm);
                        vm.push_back(v);
                        j++;

                    }

                    Process p(curProcessC, vm);
                    processes.push_back(p);
                    curProcessC++;
                } else {
                    char c;
                    int p;
                    sscanf(ch_str, "%c%d", &c, &p);
                    command cm;
                    cm.ins_type = c;
                    cm.page = p;
                    commands.push_back(cm);
                }
            }
        }

        file.close();
    } else {
        cout << "File " << file_name << " could not be opened." << endl;
        exit(0);
    }
//    cout<<commands.size()<<" "<<processes.size()<<endl;



}

Process *curProc;

void simulate(frame_t *frame_table) {
    for (command cm: commands) {
        if (oopt) {
            printf("%lu: ==> %c %d\n", ic, cm.ins_type, cm.page);
        }
        ic++;
        if (cm.ins_type == 'c') {
            cout<<"here - c"<<endl;
            curProc = &processes.at(cm.page);
            tcs++;
            continue;
        } else if (cm.ins_type == 'e') {
            cout<<"here - e"<<endl;
            printf("EXIT current process %d\n", curProc->proc_id);
            int i;
            for (i = 0; i < MAX_VPAGES; i++) {
                pte_t *p = &curProc->pageTable[i];
                if (p->getPresentBit() == 1) {
                    p->resetPresentBit() ;
                    curProc->stats->unmaps++;

                    if (oopt) {
                        printf(" UNMAP %d:%d\n", curProc->proc_id, p->getvPageNo());
                    }

//                    frame_table->[p->frameNumber].reset(NULL, NULL);
                    available.push_front(&frame_table[p->getFrameNum()]);

                    if (p->getModifiedBit() == 1) {
                        if (p->getMappedBit() == 1) {
                            curProc->stats->fouts++;
//                            curProc->fouts++;
                            printf(" FOUT\n");
                        }
                    }
                }
                p->resetPagedOutBit();
            }
            curProc = NULL;
            tpe++;
            continue;
        } else {
            cout<<"here - oth"<<endl;
            pte_t *pte = &curProc->pageTable[cm.page];
//            cout<<pte->getWriteBit()<<endl;
            if (pte->getPresentBit() == 0) {
                vma *vma;
                vma = isAvailable(cm.page, curProc);

                if (vma != NULL) {
//                    cout<<" in if vma not null"<<endl;

                    frame_t *frame = getFrame( THE_PAGER);
                    // unmap that frame from user
                    cout<<frame->vpage->getMappedBit()<<endl;
                    if (frame->vpage != NULL) {

                        cout<<"not null";
                        if (oopt) {
                            printf(" UNMAP %d:%d\n", frame->process->proc_id, frame->vpage->getvPageNo());
                        }
                        frame->process->stats->unmaps++;
//                        total_cost += 400;

                        frame->vpage->resetPresentBit();
                        cout<<"up"<<endl;
                        if (frame->vpage->getModifiedBit() == 1) {
                            // check if it was file mapped in which case write back to file (FOUT)
                            if (frame->vpage->getMappedBit() == 1) {
//                                total_cost += 2500;
                                frame->process->stats->fouts++;
                                if (oopt)
                                    printf(" FOUT\n");
                            } else {
                                frame->process->stats->outs++;
//                                total_cost += 3000;
                                frame->vpage->setPagedOutBit();
                                if (oopt)
                                    printf(" OUT\n");
                            }
                        }
                    }

                    //  map the new page to the frame
                    cout<<" new page";
                    frame->age = 0;
                    frame->tau = ic;
                    frame->reset(curProc, pte);

                    // reset the pte_t
                    pte->setPresentBit();
                    pte->setFrameNum(frame->frame);
                    pte->setReferencedBit();
                    pte->setModifiedBit();

//                    pte->mapped = vma;
//                    pte->write_protected = vma->write_protected;

                    if (pte->getPagedOutBit() == 1) {
//                        total_cost += 3000;
                        curProc->stats->ins++;
                        if (oopt)
                            printf(" IN\n");
                    } else if (pte->getMappedBit() == 1) {
                        curProc->stats->fins++;
                        if (oopt)
                            printf(" FIN\n");
                    } else {
                        curProc->stats->zeros++;
                        if (oopt)
                            printf(" ZERO\n");
                    }

                    if (oopt) {
                        printf(" MAP %d\n", frame->frame);
                    }
                    curProc->stats->maps++;

                } else {
                    curProc->stats->segv++;
                    if (oopt)
                        printf(" SEGV\n");

                    continue;
                }
            }

            if (cm.ins_type == 'r') {
                pte->setReferencedBit();
            } else if (cm.ins_type == 'w') {
                if (pte->getWriteBit() == 1) {
                    curProc->stats->segprot++;
                    if (oopt)
                        printf(" SEGPROT\n");
                    pte->setReferencedBit();
                } else {
                    pte->setModifiedBit();
                    pte->setReferencedBit();
                }
            }
            cout<<"end else"<<endl;

        }
        cout<<cm.ins_type<<endl;
    }

    if (popt) {
        for (Process p: processes) {
//            p.printPageTable();
        }
    }

    if (fopt) {
//        printFrameTable();
    }

    if (sopt) {
        for (Process p: processes) {
//            p.printSummary();
        }
        printf("TOTALCOST %lu %lu %lu %llu\n", ic, tcs, tpe, tc);
    }
}
void getBursts(string filename, deque<int> &randomNo) {
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

void initialiseFrames(frame_t *frame_table){
    for(int i=0; i<MAX_FRAMES; i++)
    {
        frame_table[i] = *(new frame_t);
        frame_table[i].frame = i;
        frame_table[i].process = NULL;
        frame_table[i].vpage = NULL;
        available.push_front(&frame_table[i]);
    }
}

frame_t * getFrame(Pager *pager )
{
    cout<<" in gt frame";
// Check if some frame is free
    if(!available.empty())
    {
        cout<<"get Frame() "<<available.size();
        frame_t *f = available.front();

        available.pop_front();
        return f;
    }

    frame_t k;
    frame_t *f = pager->select_victim_frame( 0, 64, frame_table);
    return f;
}
