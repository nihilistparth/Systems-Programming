#include "memlab.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std ; 

void Func1(data* x1, data* y1){
    
    data* arr = createArr(DT_INT, 5000) ; 
    
    for(int i = 0 ; i < 5000 ; i++){
        assignArr(arr, i, rand()%20) ; 
    }

    freeElem(arr) ; 
}

void Func2(data* x1, data* y1){
    data* arr = createArr(DT_INT, 5000) ; 
    
    for(int i = 0 ; i < 5000 ; i++){
        assignArr(arr, i, rand()%20) ; 
    }

    freeElem(arr) ; 
}

void Func3(data* x1, data* y1){
    data* arr = createArr(DT_INT, 5000) ; 
    
    for(int i = 0 ; i < 5000 ; i++){
        assignArr(arr, i, rand()%20) ; 
    }

    freeElem(arr) ; 
}

void Func4(data* x1, data* y1){
    data* arr = createArr(DT_INT, 5000) ; 
    
    for(int i = 0 ; i < 5000 ; i++){
        assignArr(arr, i, rand()%20) ; 
    }

    freeElem(arr) ; 
}

void Func5(data* x1, data* y1){
    data* arr = createArr(DT_INT, 5000) ; 
    
    for(int i = 0 ; i < 5000 ; i++){
        assignArr(arr, i, rand()%20) ; 
    }

    freeElem(arr) ; 
}

void Func6(data* x1, data* y1){
    data* arr = createArr(DT_INT, 5000) ; 
    
    for(int i = 0 ; i < 5000 ; i++){
        assignArr(arr, i, rand()%20) ; 
    }

    freeElem(arr) ; 
}

void Func7(data* x1, data* y1){
    data* arr = createArr(DT_INT, 5000) ; 
    
    for(int i = 0 ; i < 5000 ; i++){
        assignArr(arr, i, rand()%20) ; 
    }

    freeElem(arr) ; 
}

void Func8(data* x1, data* y1){
    data* arr = createArr(DT_BOOL, 5000) ; 
    
    for(int i = 0 ; i < 5000 ; i++){
        assignArr(arr, i, rand()%2) ; 
    }

    freeElem(arr) ; 
}


void Func9(data* x1, data* y1){
    data* arr = createArr(DT_CHAR, 5000) ; 
    
    for(int i = 0 ; i < 5000 ; i++){
        assignArr(arr, i, 'a' + rand()%26) ; 
    }

    freeElem(arr) ; 
}



int main(){

    createMem() ; 

    data* x1 = createVar(DT_INT, 10) ; 
    data* y1 = createVar(DT_INT, 20) ; 

    startScope() ; 

    Func1(x1, y1) ; 
    endScope() ;  

    startScope() ;
    Func2(x1, y1) ; 
    endScope() ; 

    startScope() ; 
    Func3(x1, y1) ;
    endScope() ; 

    startScope() ; 
    Func4(x1, y1) ;
    endScope() ;  

    startScope() ; 
    Func5(x1, y1) ;
    endScope() ; 

    startScope() ; 
    Func6(x1, y1) ;
    endScope() ; 

    startScope() ; 
    Func7(x1, y1) ;
    endScope() ; 

    startScope() ; 
    Func8(x1, y1) ;
    endScope() ; 

    startScope() ; 
    Func9(x1, y1) ;
    endScope() ; 

}