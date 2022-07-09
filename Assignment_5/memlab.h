#ifndef MEMLAB_H
#define MEMLAB_H

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <bits/stdc++.h>
#include <pthread.h>

using namespace std ; 
const int MEM_SIZE =  2000000; 
const int PT_SIZE  =  50000;
const int STK_SIZE =  50000;

#define DT_INT 1
#define DT_CHAR 2
#define DT_BOOL 3
#define DT_MDINT 4
#define ARR 5

class pagetable{

public:
    int offset;
    int type;
    int sz;
    int is_valid ;

    pagetable(int _offset, int _type, int _sz, int _is_valid); 

    void print(); 

};

class data{
public:
    pagetable* pt_entry;
    int type;
    int size;
    int arrayType ; 
    data(int _type, int _value) ; 
    data(int _type, char _value)  ; 
    data(int _type, bool _value) ; 
    data(int _type, int _size, int _dataType) ; 
    void printValue() ; 

    int getValue() ; 
    int getIndexValue(int index) ; 

};


class Stack{
public:
    int top ; 
    int size ; 
    data *stk[STK_SIZE] ; 

    // Push 
    void init() ; 
    void push_null(); 
    void push(data* a) ;
    int empty() ; 
    data* peek() ; 
    data* pop() ; 

}; 

class PGtable{
public:
    int maxsz = PT_SIZE ; 
    int size ; 

    pagetable* pg[PT_SIZE] ; 

    void init() ; 
    void shift_from(int index) ; 
    int search_insert(int _size,int _type); 
    void shiftLeftKtimes(int one_start, int k) ; 
    // void printTable() ; 

}; 


void createMem() ; 

data* createVar(int _type, int _value) ; 
data* createVar(int _type, char _value) ; 
data* createVar(int _type, bool _value) ; 

data* createArr(int _type, int _size) ; 

void assignVar(data* a, int _value) ;
void assignVar(data* a, char _value) ;
void assignVar(data* a, bool _value) ; 

void assignArr(data* a, int index, int _value) ;
void assignArr(data* a, int index, char _value) ;
void assignArr(data* a, int index, bool _value) ; 

void startScope() ; 
void endScope() ; 

void* garbageRunner(void* arg) ; 

void compactPTable() ; 
void defragement() ; 

void ShiftDTLeftK(int one_idx, int k, int sz) ; 

void checkFunction() ; 

void freeElem(data* a) ; 

void printPGTable() ; 

void printDT() ; 

#endif 