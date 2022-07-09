#include "memlab.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <bits/stdc++.h>
#include <unistd.h>
#include <pthread.h>

using namespace std ; 

void* ptr;
PGtable* pgT ; 
Stack* stackPT ; 
int* dataTable ;

pthread_mutex_t main_lock ; 
pthread_t tid ; 

/*************************************************************************************/

#define db(...) __f(#__VA_ARGS__, __VA_ARGS__)

template <typename Arg1>
void __f(const char* name, Arg1&& arg1) { cout << name << " : " << arg1 << '\n'; }
template <typename Arg1, typename... Args>
void __f(const char* names, Arg1&& arg1, Args&&... args) {
	const char* comma = strchr(names + 1, ',');
	cout.write(names, comma - names) << " : " << arg1 << " | "; __f(comma + 1, args...);
}


/*************************************************************************************/
pagetable::pagetable(int _offset, int _type, int _sz, int _is_valid){
    offset = _offset ; 
    type = _type ; 
    sz = _sz ; 
    is_valid = _is_valid ;  
}

void pagetable::print(){
    cout<< offset << " " << sz << " " << is_valid <<endl;
}

/*************************************************************************************/
data::data(int _type, int _value){
    if(_type == 1){
        pthread_mutex_lock(&main_lock) ;
        type = _type ; 
        size = 1 ;
        arrayType = 0 ; 
        int idx = pgT->search_insert(1, _type) ; 
        pagetable* pgentry = pgT->pg[idx] ; 
        dataTable[pgentry->offset] = _value ; 
        pt_entry = pgT->pg[idx] ;
        stackPT->push(this) ; 
        pthread_mutex_unlock(&main_lock) ; 

    }   
    else if(_type == 4){
        // medium Int

        pthread_mutex_lock(&main_lock) ;
        type = _type ; 
        size = 1 ;
        arrayType = 0 ; 
        int idx = pgT->search_insert(1, _type) ; 
        pagetable* pgentry = pgT->pg[idx] ; 
        dataTable[pgentry->offset] = _value ; 
        pt_entry = pgT->pg[idx] ;
        stackPT->push(this) ; 
        pthread_mutex_unlock(&main_lock) ; 

    }
    else{
        cout << "Error in data parameters" << endl ; 
        exit(0) ; 
    }
}

data::data(int _type, int _size, int _dataType){
    if(_type == 5){
        // char type
        pthread_mutex_lock(&main_lock) ;
        type = _dataType ; 
        arrayType = 1 ; 
        int idx = pgT->search_insert(_size, _dataType) ; 
        pagetable* pgentry = pgT->pg[idx] ; 
        size = _size ;  
        pt_entry = pgentry ;
        stackPT->push(this) ;
        pthread_mutex_unlock(&main_lock) ; 
    }
    else{
        cout << "Error in data parameters char" << endl ; 
        exit(0) ;
    }
}

data::data(int _type, char _value){
    if(_type == 2){
        // char type
        pthread_mutex_lock(&main_lock) ;
        type = _type ; 
        size = 1 ; 
        arrayType = 0 ; 
        int idx = pgT->search_insert(1, _type) ; 
        pagetable* pgentry = pgT->pg[idx] ; 
        dataTable[pgentry->offset] = _value ; 
        pt_entry = pgentry ;
        stackPT->push(this) ;
        pthread_mutex_unlock(&main_lock) ; 

    }
    else{
        cout << "Error in data parameters char" << endl ; 
        exit(0) ;
    }
}

data::data(int _type, bool _value){
    if(_type == 3){
        // bool type
        pthread_mutex_lock(&main_lock) ;
        type = _type ; 
        size = 1 ;
        arrayType = 0 ; 
        int idx = pgT->search_insert(1, _type) ; 
        pagetable* pgentry = pgT->pg[idx] ;
        if(_value == true)    dataTable[pgentry->offset] = 1 ;
        else     dataTable[pgentry->offset] = 0 ;
        pt_entry = pgentry ;
        stackPT->push(this) ;
        pthread_mutex_unlock(&main_lock) ; 
    }
    else{
        cout << "Error in data parameters bool" << endl ; 
        exit(0) ;
    }
}

int data::getValue(){
    return dataTable[pt_entry->offset] ; 
}

int data::getIndexValue(int index){
    return dataTable[pt_entry->offset + index] ; 
}

void data::printValue(){
    if(type == 1){
        cout << dataTable[pt_entry->offset] << endl ;
    }
    else if(type == 2){
        cout << (char)dataTable[pt_entry->offset] << endl ; 
    }
    else if(type == 3){
        if(dataTable[pt_entry->offset] != 0){
            cout << "TRUE" << endl ; 
        }
        else{
            cout << "FALSE" << endl ; 
        }
    }
    else if(type == 4){
        cout << dataTable[pt_entry->offset] << endl ;
    }
    else if(type == 5){
        cout << "print each element of array" << endl ; 
        for(int i = 0 ; i < size ; i++){
            cout << dataTable[pt_entry->offset + i] << " " ;
        }
        cout << endl ; 
    }
}

/*************************************************************************************/
void Stack::init(){
    top = -1 ; 
    size = STK_SIZE ; 
}

void Stack::push_null(){
    if(top == size){
        return ; 
    }
    stk[++top] = NULL ; 
}

void Stack::push(data* a){
    if(top == size){
        return ; 
    }
    stk[++top] = a ; 
}

int Stack::empty(){
    if(top != -1)   return 1 ; 
    else    return 0 ; 
}

data* Stack::peek(){
    return stk[top] ; 
}

data* Stack::pop(){
    if(this->empty())    return NULL ; 
    else return stk[top--] ; 
}

/*************************************************************************************/

void PGtable::init(){
    size = 0 ; 
    pg[size++] = new pagetable(0, 0, PT_SIZE, 0) ; 
}

void PGtable::shift_from(int index){
    if(index+2 == maxsz){
        cout << "Error in the page table size" << endl ; 
    }
    else{
        for(int i = size-1 ; i >= index ; i--){
            pg[i+1] = pg[i] ; 
        }
        size++ ; 
    }
}

int PGtable::search_insert(int _size,int _type){
    
    int best_fit_sz = PT_SIZE+1 , offset, index = -1 ; 
    for(int i = 0 ; i < size ; i++){
        if(pg[i]->is_valid == 0){
            if(best_fit_sz > pg[i]->sz && pg[i]->sz >= _size){
                best_fit_sz = pg[i]->sz ; 
                offset = pg[i]->offset ; 
                index = i ; 
            }
        }
    }
    if(best_fit_sz == PT_SIZE+1){
        cout << "Error in finding in the page table" << endl ; 
        return -1 ; 
    }
    else{
        if((pg[index]->sz - _size) > 0){
            pagetable* pt_temp = new pagetable(pg[index]->offset + _size, _type, (pg[index]->sz - _size), 0) ;
            pg[index] = pt_temp ; 
        }
        // Move the whole array 
        shift_from(index) ; 
        pg[index] = new pagetable(offset, _type, _size, 1) ; 

        return index ; 
    }
}

void PGtable::shiftLeftKtimes(int one_start, int k){

    if(k >= size){
        cout << "Error in size k shifting" << endl ; 
        exit(0) ; 
    }

    for(int i = one_start-k ; i < size-k ; i++){
        pg[i] = pg[i+k] ; 
    }
    size -= k ; 
}

/*************************************************************************************/

void createMem(){
    ptr = (void*) malloc (MEM_SIZE*sizeof(int));

    pgT = (PGtable*) malloc(sizeof(PGtable)) ; 

    stackPT = (Stack*) malloc(sizeof(Stack)) ; 
    dataTable = (int*) malloc(sizeof(int)*MEM_SIZE) ; 

    pthread_mutex_init(&main_lock, NULL) ; 
    pthread_attr_t attr; 
    pthread_attr_init(&attr) ; 

    int retG = pthread_create(&tid, &attr, garbageRunner, NULL) ;

    pgT->init() ; 

    stackPT->init() ; 

}

data* createVar(int _type, int _value){
    cout << "Creating a new int value " << endl ; 
    data* temp = new data(_type, _value) ;
    return temp ;  
}

data* createVar(int _type, char _value){
    cout << "Creating a new int char " << endl ;
    data* temp = new data(_type, _value) ;
    return temp ;
}
data* createVar(int _type, bool _value){
    cout << "Creating a new bool value " << endl ;
    data* temp = new data(_type, _value) ;
    return temp ;
} 

data* createArr(int _type, int _size){
    cout << "Creating a new array " << endl ;
    // return new data(5, _size, _type) ; 
    data* temp = new data(5, _size, _type) ;
    return temp ;
}

void assignVar(data* a, int _value){
    if (a->type != 1){
        cout << "wrong assignment done" << endl ;
        return ;  
    }
    pthread_mutex_lock(&main_lock) ;
    dataTable[(a->pt_entry)->offset] = _value ; 
    pthread_mutex_unlock(&main_lock) ;
}

void assignVar(data* a, char _value){
    if (a->type != 2){
        cout << "wrong assignment done" << endl ;
        return ;  
    }
    pthread_mutex_lock(&main_lock) ;
    dataTable[(a->pt_entry)->offset] = _value ; 
    pthread_mutex_unlock(&main_lock) ;
}

void assignVar(data* a, bool _value){
    if (a->type != 3){
        cout << "wrong assignment done" << endl ;
        return ;  
    }
    pthread_mutex_lock(&main_lock) ;
    if(_value == true)    dataTable[(a->pt_entry)->offset] = 1 ; 
    else     dataTable[(a->pt_entry)->offset] = 0 ;
    pthread_mutex_unlock(&main_lock) ;

}

void assignArr(data* a, int index, int _value){

    if (a->arrayType != 1){
        cout << "not array" << endl ;
        return ;  
    }

    if (index >= a->size){
        cout << "wrong assignment index" << endl ;
        return ; 
    } 

    pthread_mutex_lock(&main_lock) ;
    cout << "assigning the array " << index << " " << _value << endl ; 
    dataTable[(a->pt_entry)->offset + index] = _value ; 
    pthread_mutex_unlock(&main_lock) ;
    
}

void assignArr(data* a, int index, char _value){

    if (a->arrayType != 1){
        cout << "not array" << endl ;
        return ;  
    }

    if (index >= a->size){
        cout << "wrong assignment index" << endl ;
        return ; 
    } 

    pthread_mutex_lock(&main_lock) ;
    cout << "assigning the array " << index << " " << _value << endl ;
    dataTable[(a->pt_entry)->offset + index] = _value ; 
    pthread_mutex_unlock(&main_lock) ;
    
}

void assignArr(data* a, int index, bool _value){

    if (a->arrayType != 1){
        cout << "not array" << endl ;
        return ;  
    }

    if (index >= a->size){
        cout << "wrong assignment index" << endl ;
        return ; 
    } 

    pthread_mutex_lock(&main_lock) ;
    cout << "assigning the array " << index << " " << _value << endl ;
    if(_value == true)    dataTable[(a->pt_entry)->offset + index] = 1 ;
    else     dataTable[(a->pt_entry)->offset + index] = 0 ;
    pthread_mutex_unlock(&main_lock) ;
    
}

void startScope(){
    cout << "starting the scope of function" <<endl ; 
    stackPT->push_null() ; 
}

void endScope(){
    cout << "ending the scope of function" <<endl ; 
    pthread_mutex_lock(&main_lock) ; 
    while(stackPT->empty() != 1 && stackPT->peek() != NULL){
        data* curr = stackPT->pop() ; 
        curr->pt_entry->is_valid = 0 ; 
    }

    if(stackPT->empty() != 1 && stackPT->peek() == NULL){
        data* useless = stackPT->pop() ; 
    }
    pthread_mutex_unlock(&main_lock) ;
}

void* garbageRunner(void* arg){
    while(1){

        usleep(200 * 1000) ; 
        pthread_mutex_lock(&main_lock) ;
        compactPTable() ; 
        defragement() ; 

        pthread_mutex_unlock(&main_lock) ; 
    }   
}

void checkFunction(){
    compactPTable() ; 
    defragement() ;
}

void compactPTable(){
    cout << "compaction starts" << endl ;
    int comp[PT_SIZE][2] , csize = -1 ; 
    int inter[PT_SIZE][2] ; 
    for(int i = 0 ; i < pgT->size ; i++){
        if(pgT->pg[i]->is_valid == 0){
            if(csize == -1){
                csize++ ; 
                comp[csize][0] = pgT->pg[i]->offset; 
                comp[csize][1] = pgT->pg[i]->sz;

                inter[csize][0] = inter[csize][1] = i ; 
            }
            else{
                if(comp[csize][0] + comp[csize][1] == pgT->pg[i]->offset){
                    comp[csize][1] += pgT->pg[i]->sz ; 
                    inter[csize][1] = i; 
                }
                else{
                    csize++ ; 
                    comp[csize][0] = pgT->pg[i]->offset; 
                    comp[csize][1] = pgT->pg[i]->sz;

                    inter[csize][0] = inter[csize][1] = i ; 
                }
            }
        }
    }

    int j = 0 , k = 0 ; 
    for(int i = 0 ; i < pgT->size ; i++){
        if(i >= inter[k][0] && i <= inter[k][1]){
            if(i == inter[k][0]){
                pgT->pg[j]->offset = comp[k][0] ;
                pgT->pg[j]->sz = comp[k][1] ;
                pgT->pg[j]->is_valid = 0 ;
                j++ ; 
            }   
            else if(i == inter[k][1]){
                k++ ; 
                continue ; 
            }
            else{
                continue ; 
            }
        }
        else{
            pgT->pg[j]->offset = pgT->pg[i]->offset ;
            pgT->pg[j]->sz = pgT->pg[i]->sz ;
            pgT->pg[j]->is_valid = pgT->pg[i]->is_valid ;
            j++ ; 
        }
    }
    // cout << "j = " << j << endl ; 
    pgT->size = j ; 
    cout << "compaction ends" << endl ;
}

void defragement(){
    cout << "defragmentation starts" << endl ; 
    int diff = 0 , j = 0 ; 
    for(int i = 0 ; i < pgT->size ; i++){
        if(pgT->pg[i]->is_valid == 0){
            diff += pgT->pg[i]->sz ;
        }
        else{
            // Shift DataTable by curr_diff from curr_offset to end size

            ShiftDTLeftK(pgT->pg[i]->offset, pgT->pg[i]->sz, diff) ; 
            // iterate on variable list

            pgT->pg[j]->offset = pgT->pg[i]->offset - diff ;
            pgT->pg[j]->sz = pgT->pg[i]->sz ;
            pgT->pg[j]->is_valid = pgT->pg[i]->is_valid ;  

            for(int itr = 0 ; itr <= stackPT->top ; itr++){
                if(stackPT->stk[itr] == NULL){
                    continue ; 
                }
                
                if(stackPT->stk[itr]->pt_entry->offset == pgT->pg[i]->offset){
                    // db(stackPT->stk[itr]->pt_entry->offset, pgT->pg[j]->offset, itr, j) ;
                    stackPT->stk[itr]->pt_entry = pgT->pg[j] ; 
                    // db(stackPT->stk[itr]->pt_entry->offset) ; 
                    break ; 
                }
                else{
                    continue ; 
                }
            }


            j++ ;
        }
    }
    pgT->size = j ; 
    cout << "defragmentation ends" << endl ;
}

void ShiftDTLeftK(int one_idx, int sz , int k){
    // db(one_idx, sz, k) ; 
    for(int i = one_idx - k ; i < one_idx + sz-k ; i++){
        dataTable[i] = dataTable[i+k] ; 
    }

}

void printDT(){

    for(int i = 0 ; i < pgT->size ; i++){
        db(i, dataTable[i]) ; 
    }

}

void freeElem(data* a){
    cout << "freeing the element" << endl ; 
    a->pt_entry->is_valid = 0 ;
}

void printPGTable(){
    for(int i = 0 ; i < pgT->size ; i++){
        cout << "page table entry " <<i << " " ; 
        pgT->pg[i]->print() ; 
    }
}