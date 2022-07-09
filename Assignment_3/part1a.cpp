#include<stdio.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<sys/types.h>
#include <iostream>
#include<sys/stat.h>
#include <stdlib.h>
#include <string.h>


using namespace std ; 


typedef struct _process_data{

    double **A ; 
    double **B ; 
    double **C ; 

    int veclen, i, j, lim ; 

} ProcessData ; 


double **A ; 
double **B ; 

void mult(ProcessData *p){

    int i = p->i , j = p->j ;
    int c2 = p->lim;
    for(int k = 0 ; k < p->veclen ; k++){
        ((*(double *)(p->C + i*c2 + j)) ) += ((p->A[i][k]) * (p->B[k][j])) ; 
    }
    return  ; 
}

int main(){

    int r1, c1, r2, c2 ; 

    cout << "Enter the dimensions and values of first matrix" << endl ; 

    cin >> r1 >> c1 ;
    
    A = new double*[r1] ; 
    
    for(int i = 0 ; i < r1 ; i++){
        A[i] = new double[c1] ; 
    }

    for(int i = 0 ; i < r1 ; i++){
        for(int j = 0 ; j < c1 ; j++){
            cin >> A[i][j] ; 
        }
    }

    cout << "Enter the dimensions and value of second matrix" << endl ; 

    cin >> r2 >> c2 ; 

    B = new double*[r2] ; 
    
    for(int i = 0 ; i < r2 ; i++){
        B[i] = new double[c2] ; 
    }

    for(int i = 0 ; i < r2 ; i++){
        for(int j = 0 ; j < c2 ; j++){
            cin >> B[i][j] ; 
        }
    }

    if(r2 != c1){
        cout << "Cannot be multiplied" << endl ;
        for(int i = 0 ; i < r1 ; i++)   delete [] A[i] ;
        for(int i = 0 ; i < r2 ; i++)   delete [] B[i] ;
        delete [] A ; 
        delete [] B ;  
        return 0 ;  
    }
    
    int key = 1221;
    int shmid = shmget(key, r1*c2*sizeof(double*),IPC_CREAT|0666) ; 
    
    double ** _C = (double **)(shmat(shmid, NULL, 0)) ; 
    for(int i = 0;i<r1;i++){
        for(int j =0;j<c2;j++){
           
            *((double *)(_C + i*c2 + j)) = 0;
        }
    }    
    int status ; 
   
    for(int i = 0 ; i < r1 ; i++){
       
        for(int j = 0 ; j < c2 ; j++){
            pid_t child = fork() ;            
            if(child<0){
                cout<<"ERROR"<<endl;
                exit(0);
            }
            if(child == 0){   
                
                if(shmid<0){
                    cout<<"ERROR shmid"<<endl;
                    exit(0);
                }
                ProcessData* p  = new ProcessData(); 
                p->A = A ; 
                p->B = B ;
                p->C = _C ;
                p->veclen = c1 ; 
                p->i = i ; 
                p->j = j ;
                p->lim = c2;
                mult(p) ;  
              *((double *)(_C + i*c2 + j)) = p->C[i][j];               
                exit(0);
            }
               
        }   

    }
    while(wait(NULL)>0);
    for(int i = 0 ; i < r1 ; i++){
        for(int j = 0 ; j < c2 ; j++){
            cout <<  *((double *)(_C + i*c2 + j)) << " " ; 
        }
        cout << endl ; 
    }


    shmdt(_C) ; 
    shmctl(shmid, IPC_RMID, NULL) ; 

    for(int i = 0 ; i < r1 ; i++)   delete [] A[i] ;
    for(int i = 0 ; i < r2 ; i++)   delete [] B[i] ;
    delete [] A ; 
    delete [] B ;  

    return 0 ; 

}   



