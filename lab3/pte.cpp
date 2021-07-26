//
// Created by Nidhi Ranjan on 26/07/21.
//
#include<iostream>

#pragma once
using namespace std;

class pte_t {
    unsigned present: 1;
    unsigned referenced: 1;
    unsigned modified: 1;
    unsigned write_protected: 1;
    unsigned pagedeout: 1;
    unsigned frame_no: 7;
    unsigned mapped: 1;
    unsigned vPageNo: 6;
    unsigned vmaSet: 1;
    unsigned vma: 12;
public :
    pte_t() {
        this->present = 0;
        this->referenced = 0;
        this->modified = 0;
        this->write_protected = 0;
        this->pagedeout = 0;
        this->frame_no = 0;
        this->mapped = 0;
        this->vPageNo = 0;
        this->vma = 0;
        this->vmaSet = 0;


    }


    unsigned int getFrameNum() {
        return this->frame_no;
    }

    void setFrameNum(unsigned int a) {
        this->frame_no = a;
    }

    void resetFrameNum() {
        this->frame_no = 0;
    }

    unsigned int getPresentBit() {
        return this->present;
    }

    void setPresentBit() {
        this->present = 1;
    }

    void resetPresentBit() {
        this->present = 0;
    }

    unsigned int getReferencedBit() {
        return this->referenced;
    }

    void setReferencedBit() {
        this->referenced = 1;
    }

    void resetReferencedBit() {
        this->referenced = 0;
    }

    unsigned int getModifiedBit() {
        return this->modified;
    }

    void setModifiedBit() {
        this->modified = 1;
    }

    void resetModifiedBit() {
        this->modified = 0;
    }

    unsigned int getPagedOutBit() {
        return this->pagedeout;
    }

    void setPagedOutBit() {
        this->pagedeout = 1;
    }

    void resetPagedOutBit() {
        this->pagedeout = 0;
    }

    unsigned int getMappedBit() {
        return this->mapped;
    }

    void setMappedBit(unsigned int a) {
        this->mapped = a;
    }

    unsigned int getvPageNo() {
        return this->vPageNo;
    }

    void setvPageNo(unsigned  int a) {
        this->vPageNo = a;
    }
    void resetvPageNo() {
        this->vPageNo = 0;
    }

    unsigned int getWriteBit() {
        return this->write_protected;
    }

    void setWriteBit() {
        this->vPageNo = 1;
    }
    void resetWriteBit() {
        this->vPageNo = 0;
    }
    unsigned int getVMAset(){
    return this->vmaSet;
}
    unsigned int getVMA(){
        return this->vma;
    }
    void setVMAset(){
    this->vmaSet = 1;
}

};