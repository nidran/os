//
// Created by Nidhi Ranjan on 26/07/21.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <queue>
#include <stack>
#include <algorithm>
#include <getopt.h>
#include <climits>
using namespace std;
//PAGE TABLE ENTRY -- Planned on using it but ended up with bit manipulation


//Rachit Mehrotra
//rm4149@nyu.edu


class PTE
{
public:
    unsigned int present:1;
    unsigned int modified:1;
    unsigned int referenced:1;
    unsigned int pagedout:1;
    unsigned int indexPhysical:6;
    PTE()
    {
        present=modified=referenced=pagedout=0;
        indexPhysical=0;
    }

};

// Variable Declarations
int frameCount=32;//Default value

int totalRandomValues = 0;
int instructions = 0;
int unmapped = 0;
int mapped = 0;
int in = 0;
int out = 0;
int zero = 0;
std::vector<unsigned int> _pageVec = std::vector<unsigned int>(64, 0);
vector<PTE> pageV= vector<PTE>(64,PTE());
std::vector<unsigned int> _frames_table;
std::vector<unsigned int> _rev_frames_table;
unsigned int limit=0;


bool O=false,F=false,P=false,S=false;

int * random_num=NULL;;
int ofs = 0;

// frame manipulation to get frame value / set frame value in a page
unsigned int frameManip(unsigned int& page , int frame , int type)
{
    if(type==1)
    {
        page=(page & 0xf0000000);
        page |= frame;
        return 0;
    }
    else if (type == 2 )
    {
        return ((page << 4) >> 4);
    }

}
// bit manipulation to set / reset specific bit's in a page table entry unsigned it value -> current bit , mod bit ,reference bit and paged out bit is
//implemented here using unsigned int. first 4 bits are for these entries and
unsigned int bitManip(unsigned int page, int s_r , int bit)
{
    if(s_r == 1)
    {   unsigned int temp;
        if (bit==0)
        {   //current bit
            temp = 0x80000000;
            page |= temp;
            return page;

        }
        if (bit==1)
        {  //mod bit
            temp= 0x40000000;
            page |= temp;
            return page;

        }
        if (bit==2)
        { //ref bit
            temp = 0x20000000;
            page |= temp;
            return page;

        }
        if (bit==3)
        { // out bit
            temp = 0x10000000;
            page |= temp;
            return page;
        }
    }
    else if (s_r==0)
    { unsigned int temp;
        if (bit==0)
        {   //current bit
            temp = 0x7fffffff;
            page &= temp;
            return page;

        }
        if (bit==1)
        {  //mod bit
            temp= 0xbfffffff;
            page &= temp;
            return page;

        }
        if (bit==2)
        { //ref bit
            temp = 0xdfffffff;
            page &= temp;
            return page;

        }
    }

}


// get/extract BIT from page_entry , as 31st bit is current, 20th is modified , 29th is reference and 28th is pagedout
unsigned int getBITS(unsigned int page_entry , string bit)
{
    if (bit=="current")
    {
        page_entry=page_entry >> 31 ;
        page_entry |= 0;
        return page_entry;
    }
    else if (bit=="modified")
    {
        page_entry= page_entry >> 30 ;
        page_entry &= 1 ;
        page_entry |= 0;
        return page_entry;
    }
    else if (bit=="referenced")
    {

        page_entry= page_entry >> 29 ;
        page_entry &= 1 ;
        page_entry |= 0;
        return page_entry;

    }
    else if (bit=="pagedout")
    {
        page_entry= page_entry >> 28 ;
        page_entry &= 1 ;
        page_entry |= 0;
        return page_entry;
    }
}

int num_of_random_val=0;
void initializeRandom(char * Fl){

    ifstream fin;
    fin.open(Fl);
    string str;
    if(fin.is_open()){
        getline(fin,str);
        num_of_random_val=atoi(str.c_str());
        random_num=new int[num_of_random_val];
        int count=0;
        while(getline(fin,str)){
            random_num[count]=atoi(str.c_str());
            count++;
        }
    }

    fin.close();
}
int getRandomNumber(int burst){
    if(ofs == num_of_random_val)
        ofs = 0;
    int returnVal = random_num[ofs] % burst;
    ofs++;
    return returnVal;
}
class Instructions
{ public:
    int ins_num;
    int rw;
    unsigned int page_num;
public:
    Instructions(int i,int a,int b)
    {
        this->rw=a;
        this->page_num=b;
        this->ins_num=i;
    }
    void toString()
    {
        cout<<"Instrc numner: "<<this->ins_num<<" R/W:"<<this->rw<<" Page_num:"<<this->page_num;
    }

};

// main MMU pagetable_algo Parents class --> for implementing OOPS concepts
class pagetable_algo
{
public:
    pagetable_algo(){};
    virtual void updatePage(unsigned int){};
    virtual ~pagetable_algo(){};
    virtual int getPage(){};

};
pagetable_algo* algo;

//-------------------NRU-------------------
class nru : public pagetable_algo
{
public:
    int count=0;
    int nru_replacement=10;

    int getPage() {
// vector<vector<int> > priority(4, vector<int>());
        vector<int> priority0;
        vector<int> priority1;
        vector<int> priority2;
        vector<int> priority3;
        bool flag=false;
        int returnPage = -1;
        for (int i = 0; i < _pageVec.size(); i++) {
            flag=false;
            unsigned int p = _pageVec[i];
            if (getBITS(_pageVec[i],"current") == 1) {
                unsigned int ref = getBITS(p,"referenced");
                unsigned int mod = getBITS(p,"modified");

                if (ref == 0 && mod == 0) {
                    priority0.push_back(frameManip(p,0,2));
                } else if (ref == 0 && mod == 1) {
                    priority1.push_back(frameManip(p,0,2));
                } else if (ref == 1 && mod == 0) {
                    priority2.push_back(frameManip(p,0,2));
                } else if (ref == 1 && mod == 1) {
                    priority3.push_back(frameManip(p,0,2));
                }
            }
        }

        // for (int i = 0; i <= 3; i++) {
        if (priority0.size() <= 0) {

        }
        else {
            int rand = getRandomNumber(priority0.size());
            returnPage = priority0[rand];
            flag=true;
        }
        if(!flag)
            if (priority1.size() <= 0) {

            }
            else {
                int rand = getRandomNumber(priority1.size());
                returnPage = priority1[rand];
                flag=true;
            }
        if(!flag)
            if (priority2.size() <= 0) {

            }
            else {
                int rand = getRandomNumber(priority2.size());
                returnPage = priority2[rand];
                flag=true;
            }
        if(!flag)
            if (priority0.size() <= 0) {

            }
            else {
                int rand = getRandomNumber(priority3.size());
                returnPage = priority3[rand];
                flag=true;
            }
        // }

        count++;
        if (count == nru_replacement) {
            count = 0;
            int i=0;
            while (i<_pageVec.size())
            {
                if (getBITS(_pageVec[i],"current") == 1) {
                    _pageVec[i]=bitManip(_pageVec[i],0,2);
                }
                i++;
            }
        }

        return returnPage;
    }


};
//-------------------LRU-------------------
class lru : public pagetable_algo
{
public:
    void updatePage(unsigned int num){
        for (int i = 0; i < _frames_table.size(); i++)
        {
            if (_frames_table[i] == num) {
                _frames_table.erase(_frames_table.begin() + i);
                break;
            }
        }
        _frames_table.push_back(num);
    }

    int getPage() {
        unsigned int frameNumber = _frames_table.front();
        _frames_table.erase(_frames_table.begin());
        _frames_table.push_back(frameNumber);
        return _frames_table.back();
    }
};
//-------------------randoms-------------------
class randoms : public pagetable_algo
{
public:
    int getPage(){ return _frames_table[getRandomNumber(_frames_table.size())];}

};
//-------------------FIFO-------------------
class fifo : public pagetable_algo
{
public:
    int getPage(){
        unsigned int temp = _frames_table.front();
        _frames_table.erase(_frames_table.begin());
        _frames_table.push_back(temp);
        temp=_frames_table.back();
        return temp;
    }
};
//-------------------SecondChance-------------------
class secondchance : public pagetable_algo
{
public:
    int getPage(){
        int flag = 0;
        unsigned int frameNum, pg;

        while (!flag)
        {
            frameNum = _frames_table.front();
            if (getBITS(_pageVec[_rev_frames_table[frameNum]],"referenced") != 1)
            {
                _frames_table.erase(_frames_table.begin());
                _frames_table.push_back(frameNum);
                flag = 1;
            }
            else
            {
                _pageVec[_rev_frames_table[frameNum]]=bitManip(_pageVec[_rev_frames_table[frameNum]],0,2);
                _frames_table.erase(_frames_table.begin());
                _frames_table.push_back(frameNum);
            }

        }
        return _frames_table.back();
    }
};
//-------------------Clock-Physical-------------------
class clockphysical : public pagetable_algo
{
public:
    int count=0;
    int getPage()
    {
        int flag = 0;
        unsigned int frameNum;

        while (!flag) {
            frameNum = _frames_table[count];


            if (getBITS(_pageVec[_rev_frames_table[frameNum]],"referenced") != 1)
            {flag = 1;

            } else {
                _pageVec[_rev_frames_table[frameNum]]=  bitManip(_pageVec[_rev_frames_table[frameNum]],0,2);
            }
            count = (count + 1) % _frames_table.size();
        }
        return frameNum;
    }
};
//-------------------Clock-Virtual-------------------
class clockvirtual : public pagetable_algo
{
public:
    int count=0;
    int getPage(){
        int flag = 1;
        unsigned int ret;

        while (flag) {
            if (getBITS(_pageVec[count],"current"))
            {
                if (getBITS(_pageVec[count],"referenced"))
                    _pageVec[count]=bitManip(_pageVec[count],0,2);
                else
                {
                    flag = 0;
                    ret = frameManip(_pageVec[count],1,2);
                }
            }
            count = ((count + 1) % _pageVec.size());
        }

        return ret;
    }
};
//-------------------Aging-Physical-------------------
class agingphysical : public pagetable_algo
{
public:
    vector<unsigned int> ageVector;
    int getPage(){
        unsigned int min_aging = -1;//to get two's complement of the data !!
        unsigned int min_frame = -1;
        unsigned int min_Index = -1;
        unsigned int temp_val;
        if (ageVector.size() == 0)
            ageVector = std::vector<unsigned int>(frameCount, 0);

        for (int i = 0; i < _frames_table.size(); i++) {
            temp_val=getBITS(_pageVec[_rev_frames_table[_frames_table[i]]],"referenced");
            ageVector[i] = (ageVector[i] >> 1);
            ageVector[i] |= (temp_val << 31);
            _pageVec[_rev_frames_table[_frames_table[i]]]=bitManip(_pageVec[_rev_frames_table[_frames_table[i]]],0,2);
            if (ageVector[i] < min_aging) {
                min_aging = ageVector[i];
                min_frame = _frames_table[i];
                min_Index = i;
            }
        }
        ageVector[min_Index] = 0;

        return min_frame;

    }
};
//-------------------Aging-Virtual-------------------
class agingvirtual : public pagetable_algo
{
public:
    vector<unsigned int> ageVector;
    int getPage(){
        unsigned int min_aging = 0xffffffff;
        unsigned int min_Index = -1;


        if (ageVector.size() == 0) {
            ageVector = std::vector<unsigned int>(64, 0);
        }

        for (int i = 0; i < _pageVec.size(); i++) {
            unsigned int pg = _pageVec[i];
            ageVector[i]= (ageVector[i] >> 1);
            ageVector[i] |= (getBITS(pg,"referenced") << 31);

            if (getBITS(pg,"current") == 1 && ageVector[i] < min_aging) {
                min_aging = ageVector[i];
                min_Index = i;
            }

            if (getBITS(pg,"current") == 1) {
                _pageVec[i]=bitManip(_pageVec[i],0,2);
            }
        }
        ageVector[min_Index] = 0;


        return frameManip(_pageVec[min_Index],0,2);
    }
};
// Simulation function that takes op code (0/1) and page number and runs !!
void mapPageFrame(unsigned int op, unsigned int pg) {

    unsigned int& page = _pageVec[pg];
    // cout<<"Value of page is --->"<<page<<endl;
    if (O==true)
        printf("==> inst: %d %d\n", op, pg);
    unsigned int num;

    if (getBITS(page,"current") == 0)
    {
        if(_frames_table.size()>= frameCount)
        {
            num=algo->getPage();

            unsigned int& previousPage = _pageVec[ _rev_frames_table[num]];
            if (O==true)
                printf("%d: UNMAP%4d%4d\n", instructions,  _rev_frames_table[num], num);

            unmapped+=1;

            previousPage=bitManip(previousPage,0,0);
            previousPage=bitManip(previousPage,0,2);

            if(getBITS(previousPage,"modified")==1)
            {
                previousPage=bitManip(previousPage,0,1);
                previousPage=bitManip(previousPage,1,3);
                if (O==true)
                    printf("%d: OUT  %4d%4d\n", instructions,  _rev_frames_table[num],num);

                out++;

            }

            if(getBITS(page,"pagedout")==1)
            {
                in++;
                if (O)
                    printf("%d: IN   %4d%4d\n", instructions, pg, num);
            }
            else
            {
                zero++;
                if (O)
                    printf("%d: ZERO     %4d\n", instructions, num);
            }

            mapped++;
            if (O)
                printf("%d: MAP  %4d%4d\n", instructions, pg, num);

            frameManip(page, num,1);
            _rev_frames_table[num] = pg;
        }


        else
        {
            num=_frames_table.size();
            frameManip(page, num,1);
            zero+=1;
            mapped+=1;
            if (O)
            {
                printf("%d: ZERO     %4d\n%d: MAP  %4d%4d\n", instructions, num,instructions, pg, num);
                // printf("%d: MAP  %4d%4d\n", instructions, pg, num);
            }
            _frames_table.push_back(num);
            _rev_frames_table[num] = pg;
        }

        page=bitManip(page,1,0);

        if (op == 0) {
            page=bitManip(page,1,2);
        } else {
            page=bitManip(page,1,2);
            page=bitManip(page,1,1);
        }


    }
    else
    {                 algo->updatePage(frameManip(page,0,2));
        if (op != 0) {
            page=bitManip(page,1,2);
            page=bitManip(page,1,1);
        } else {
            page=bitManip(page,1,2);

        }
    }


    instructions++;

}
// PRINT P FLAG
void print_P()
{
    for (int i = 0; i < _pageVec.size(); ++i) {
        if (getBITS(_pageVec[i],"current")) {
            printf("%d:", i);
            if (getBITS(_pageVec[i],"referenced"))
                printf("R");
            else
                printf("-");


            if (getBITS(_pageVec[i],"modified"))
                printf("M");
            else
                printf("-");


            if (getBITS(_pageVec[i],"pagedout"))
                printf("S ");
            else
                printf("- ");


        } else {
            if (getBITS(_pageVec[i],"pagedout"))
                printf("# ");
            else
                printf("* ");

        }
    }
    printf("\n");
}

vector <Instructions> _instructionVector;
// READ INPUT FROM USER -- Arguments passing
void input_get (int argc, char **argv)
{
    int index=0;
    int c=0;
    bool algo_flag=false;
    bool invalid_algo=false;
    opterr = 0;
    while ((c = getopt (argc, argv, "a:o:f:")) != -1)
        switch (c)
        {
            case 'a':
                algo_flag=true;
                if(optarg[0] == 'N')
                    algo = new nru();
                else if(optarg[0] == 'l')
                    algo = new lru();
                else if(optarg[0] == 'r')
                    algo = new randoms();
                else if(optarg[0] == 'f')
                    algo = new fifo();
                else if(optarg[0] == 's')
                    algo = new secondchance();
                else if(optarg[0] == 'c')
                    algo = new clockphysical();
                else if(optarg[0] == 'X')
                    algo = new clockvirtual();
                else if(optarg[0] == 'a')
                    algo = new agingphysical();
                else if(optarg[0] == 'Y')
                    algo = new agingvirtual();
                else
                {
                    invalid_algo=true;
                }
                break;


            case 'o':
                for(int i=0; optarg[i]!='\0'; i++)
                {
                    if(optarg[i] == 'O')
                        O = true;
                    else if(optarg[i] == 'F')
                        F = true;
                    else if(optarg[i] == 'P')
                        P = true;
                    else if(optarg[i] == 'S')
                        S = true;
                }
                break;
            case 'f':
                frameCount = atoi(optarg);
                break;

            default: cout<<"Invalid Arguments!";
        }

    if(!algo_flag)
    {
        if(!invalid_algo)
        {
            algo=new lru();
        }
    }
// READING INPUT FILE AND RANDOM FILE
    for (index = optind; index < argc; index++)
    {
        if(index==argc-2)

        {
            // cout<<"INPUT FILE IS "<<argv[index]<<endl;
            ifstream infile(argv[index]);
            int a, b;
            int i=1;
            string l;
            while (getline(infile,l))
            {
                if(l.at(0)!='#')
                {
                    int a=atoi((l.substr(0,1)).c_str());
                    //MAKE SURE b is between 0..63 inclusive
                    unsigned int b=atoi((l.substr(2)).c_str());
                    // Instructions temp=Instructions(i,a,b);
                    // mapPageFrame(temp.rw, temp.page_num);
                    _instructionVector.push_back(Instructions(i,a,b));
                    i++;
                }


            }
            //printf ("Non-option argument 1 %s\n", argv[index]);
        }
        if (index==argc-1)
        {
            // cout<<"RANDOM FILE IS "<<argv[index]<<endl;
            initializeRandom(argv[index]);
            // printf ("Non-option argument 2 %s\n", argv[index]);
        }
    }
    _rev_frames_table = std::vector<unsigned int>(frameCount, 0);


}



//PRINT F FLAG
void print_F()
{
    for (int i = 0; i < _rev_frames_table.size(); ++i) {
        if (_rev_frames_table[i] == -1) {
            printf("* ");
        } else {
            printf("%d ", _rev_frames_table[i]);
        }
    }
    printf("\n");
}
// PRINT SUM-S flag
void print_sum()
{
    unsigned long long cost;

    cost = (long long)instructions + 400 * (long long)(mapped + unmapped) + 3000 * (long long)(in + out) + 150 * (long long)zero;
    printf("SUM %d U=%d M=%d I=%d O=%d Z=%d ===> %llu\n",instructions, unmapped, mapped, in, out, zero, cost);
}





int main(int argc , char **argv)
{
    input_get(argc,argv);

    for (int i = 0; i < _instructionVector.size(); i++) {
        Instructions inst = _instructionVector[i];
        // cout<<"Calling mappageframe for time -->"<<i<<endl;
        mapPageFrame(inst.rw, inst.page_num);
    }

    if(P==true)
    {
        print_P();
    }
    if(F==true)
    {
        print_F();
    }
    if(S==true)
    {
        print_sum();
    }



    return 0;
}