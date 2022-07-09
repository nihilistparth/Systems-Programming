#include "memlab.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std ; 

int main(){
    createMem() ; 
    int k = 10 ; 
    data* lim = createVar(DT_INT, k) ; 

    data* arr = createArr(DT_INT, k+2) ; 

    assignArr(arr, 0, 0) ; 
    assignArr(arr, 1, 1) ; 

    data *ans = createVar(DT_INT, 1) ; 

    for(int i = 2 ; i <= k ; i++){
        data* val = createVar(DT_INT, arr->getIndexValue(i-1) + arr->getIndexValue(i-2))  ; 
        assignArr(arr, i, val->getValue()) ;

        assignVar(ans, ans->getValue() * arr->getIndexValue(i) ) ;  
    }

    ans->printValue() ; 

}