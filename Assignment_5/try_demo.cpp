#include "memlab.h"

using namespace std ; 

int main(){
    
    createMem() ; 

    data* a = createVar(1, 10) ; 
    data* b = createVar(1, 20) ; 
    data* c = createVar(1, 30) ; 
    cout << "done till " << endl ; 

    (a->pt_entry)->print() ; 
    (b->pt_entry)->print() ;
    (c->pt_entry)->print() ;

    cout << "done till 1" << endl ; 
    
    (a->printValue()) ;
    (b->printValue()) ;
    (c->printValue()) ; 

    cout << "done till 2" << endl ; 

    assignVar(a, 15) ; 
    a->printValue() ; 

    data* e = createVar(2, 'a') ; 
    e->printValue() ; 

    data* f = createVar(3, true) ; 
    f->printValue() ;

    data* g = createVar(1, 25) ; 
    g->printValue() ;

    (e->pt_entry)->print() ; 
    (f->pt_entry)->print() ;


    e->printValue() ; 

    freeElem(c) ; 

    (a->pt_entry)->print() ; 
    (b->pt_entry)->print() ;
    (c->pt_entry)->print() ;
    (e->pt_entry)->print() ; 
    (f->pt_entry)->print() ;
    (g->pt_entry)->print() ;
    printPGTable() ; 

    cout << "ddone 1" << endl ; 
    printDT() ; 

    // checkFunction() ; 

    printPGTable() ;

    printDT() ; 

    cout << "done 2" << endl ; 
    (a->pt_entry)->print() ; 
    (b->pt_entry)->print() ;
    (c->pt_entry)->print() ;
    (e->pt_entry)->print() ; 
    (f->pt_entry)->print() ;
    (g->pt_entry)->print() ;
    
    cout << "print3" << endl ;
    // c->printValue() ; 
    e->printValue() ; 
    f->printValue() ; 
    g->printValue() ; 
    
}