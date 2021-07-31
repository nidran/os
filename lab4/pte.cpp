
#include<iostream>

#pragma once

using namespace std;

class pte_t
{
public:
    unsigned present:1;
    unsigned write_protected:1;
    unsigned modified:1;
    unsigned referenced:1;
    unsigned pagedout:1;
    unsigned frameNumber:7;
    unsigned fileMapped:1;
    unsigned vPageNo:6;
    unsigned used:1;
    unsigned vma:12;


    pte_t()
    {
        present = 0;
        write_protected  = 0;
        referenced = 0;
        modified = 0;
        pagedout = 0;
        frameNumber = 0;
        fileMapped = 0;
        used = 0;
        vma = 0;
    }

   void resetPTE( unsigned long fno, unsigned long mapped, unsigned long write){
        this->present = 1;
        this->frameNumber = fno;
        this->referenced = 0;
        this->modified = 0;
        this->fileMapped = mapped;
        this->write_protected = write;
    }
};
