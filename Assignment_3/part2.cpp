#include <fstream>
#include <ctype.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fcntl.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

using namespace std ; 

#define MAX_QUEUE_SIZE 12
#define MAX_JOB_ID 100000
#define N 500

#define db(...) __f(#__VA_ARGS__, __VA_ARGS__)
 
template <typename Arg1>
void __f(const char* name, Arg1&& arg1) { cout << name << " : " << arg1 << '\n'; }
template <typename Arg1, typename... Args>
void __f(const char* names, Arg1&& arg1, Args&&... args) {
    const char* comma = strchr(names + 1, ',');
    cout.write(names, comma - names) << " : " << arg1 << " | "; __f(comma + 1, args...);
}

typedef struct _matrix{
    pid_t prod_Id ; 
    int prod_no, status, mId; 
    int mat[N][N] ; 
}Matrix ; 

typedef struct _SharedMemory{
    Matrix matQ[MAX_QUEUE_SIZE] ; 
    int size, ind , job_created, job_completed ; 
    int num_matrix ; 

    sem_t mutex;
	sem_t full;
	sem_t empty;

}SharedMemory ; 

//*************************************************************************
// FOR SIMPLCITY WE HAVE PRINTED ONLY THE STARTING 4X4 CELL OF THE MATRIX IN PRINT MATRIX FUNCTION
void init_SHM(SharedMemory* shm, int num_matrix){

    
    shm->num_matrix = num_matrix ; 

    shm->job_created = 0 ; 
    shm->job_completed = 0 ;
    shm->size = 0 ;
    int i = 0; 
    for(i = 0 ; i < MAX_QUEUE_SIZE ; i++){
        shm->matQ[i].status = -1 ; 
    }
    shm->size = 0 ;
    shm->ind = 0 ; 
    // return shm ; 

    int sema = sem_init(&(shm->mutex),1,1);
	//counting semaphore to check if the priority_queue is full
	int full_sema = sem_init(&(shm->full),1,0);
	//counting semaphore to check if the priority_queue is empty
	int empty_sema= sem_init(&(shm->empty),1,MAX_QUEUE_SIZE);
	if(sema<0||full_sema<0||empty_sema<0){
		printf("Error in initializing semaphore. Exitting..\n");
		exit(1);
	}


}

void insert_matrix(SharedMemory *shm, Matrix m){
    while(shm->size == MAX_QUEUE_SIZE)
        ;
    shm->matQ[shm->size] = m ; 
    shm->size++ ; 
}

Matrix deleteMatrix(SharedMemory* shm){
    while(shm->size < 2)
        ; 
    int i = 0 ;
    Matrix m = shm->matQ[0] ;  
    while(i < (shm->size - 1)){
        shm->matQ[i] = shm->matQ[i+1] ; 
        i++ ; 
    }
    shm->matQ[i].status = -1 ; 
    shm->size-- ; 
    return m ; 
}

// **************************************************************************

// void delay(){
//     sleep(0.1) ; 
// }

Matrix createMatrix1(pid_t prod_pid, int prod_no){

    Matrix m ;  

    m.prod_Id = prod_pid ; 
    m.prod_no = prod_no ;  
    m.mId = rand()%MAX_JOB_ID + 1 ;

    for(int i = 0 ; i < N ; i++){
        for(int j = 0 ; j < N ; j++){
            m.mat[i][j] = (rand()%18) - 9 ; 
        }
    }
    return m ;  

}

Matrix createMatrix2(pid_t prod_pid, int prod_no){

    Matrix m ;  

    m.prod_Id = prod_pid ; 
    m.prod_no = prod_no ;  
    m.mId = rand()%MAX_JOB_ID + 1 ;

    for(int i = 0 ; i < N ; i++){
        for(int j = 0 ; j < N ; j++){
            m.mat[i][j] = 0 ; 
        }
    }
    return m ;  

}

void printMatrix(Matrix m){
    cout << "Matrix details are" << endl ; 
    cout << m.prod_Id << endl ; 
    cout << m.prod_no << endl ; 
    cout << m.mId << endl ; 
    for(int i = 0 ; i < 4 ; i++){
        for(int j = 0 ; j < 4 ; j++){
            cout << m.mat[i][j] << " " ; 
        }
        cout << endl ; 
    }

}

void muliplyMatrix(int res[N][N], int a1[N][N], int a2[N][N], int status){
    int c1, c2 ,c3 ; 
    c3 = status%2 ; 
    c2 = ((status)/2)%2 ; 
    c1 = status/4 ; 
    c3 *= (N/2) ; 
    c2 *= (N/2) ;
    c1 *= (N/2) ;
    for(int i = 0 ; i < N/2 ; i++){
        for(int j = 0 ; j < N/2 ; j++){
            for(int k = 0 ; k < N/2 ; k++){
                res[i+c1][j+c2] += (a1[i+c1][k+c3] * a2[k+c3][j+c2]) ; 
            }
        }
    }

}

// **************************************************************************

void producer(SharedMemory *shm, pid_t prod_id , int prod_no, int num_matrix){
    while(true){
        if(shm->job_created == num_matrix){
            break ; 
        }
        // sleep(rand()%4) ; 
        Matrix m = createMatrix1(prod_id, prod_no) ; 

        sleep(rand()%4) ; 
        sem_wait(&(shm->empty)) ; 
        sem_wait(&(shm->mutex)) ; 
        if(shm->job_created == num_matrix){
            sem_post(&(shm->mutex)) ; 
            break ; 
        }
        
        if(shm->size < MAX_QUEUE_SIZE-1){
            // Matrix m = createMatrix(prod_id, prod_no) ;        
            // db("isnert print") ; 
			insert_matrix(shm,m);
            // db("out insert") ; 
			// printf("Produced matrix details:\n");
			printMatrix(m);
			// increment shared variable
			shm->job_created++;
			// signal the full semaphore		
			sem_post(&(shm->full));						
		}
        sem_post(&(shm->mutex));
    }

}


void worker(SharedMemory* shm, pid_t work_id, int work_no, int num_matrix){

    while(1){

        sleep(rand()%4);
		// wait for the full semaphore
		sem_wait(&(shm->full));
		// wait to acquire mutex
		sem_wait(&(shm->mutex));

        if(shm->job_completed == num_matrix-1){
            sem_post(&(shm->mutex)) ; 
            break ; 
        }
        else if(shm->job_completed < num_matrix-1 && shm->size >= 2){

            int st1 = shm->matQ[0].status ; 
            int st2 = shm->matQ[1].status ; 
            
            if(st1 != st2){
                cout << "ERROR in worker calculation side" << endl ; 
                sem_post(&(shm->mutex)) ;
                break ; 
            }
            else{   
                
                
                if(st1 == 0){
                    // multiply with status = 0
                    int k = shm->size ;
                    if(k == MAX_QUEUE_SIZE || k < 2){
                        cout << "ERROR in worker calculation side and size" << endl ; 
                        sem_post(&(shm->mutex)) ;
                        break ; 
                    }
                    Matrix m = createMatrix2(work_id, work_no) ; 
                    shm->ind = k ; 
                    sem_post(&(shm->full)) ; 
                    insert_matrix(shm, m) ;  
                    sem_post(&(shm->full)) ; 

                    muliplyMatrix(shm->matQ[k].mat, shm->matQ[0].mat, shm->matQ[1].mat, 0) ;   
                    cout << "worker work " << work_no << " done " << endl ; 
                    cout << "status " << st1 << endl ;      
                    sem_post(&(shm->full));	

                    (shm->matQ[0].status)++ ; 
                    (shm->matQ[1].status)++ ; 
                    sem_post(&(shm->full));	
                }
                else if(st1 == 1){
                    // multiply with status = 1
                    int k = shm->ind ; 
                    if(k == MAX_QUEUE_SIZE || k < 2){
                        cout << "ERROR in worker calculation side and size" << endl ; 
                        sem_post(&(shm->mutex)) ;
                        break ; 
                    }
                    shm->ind = k ;

                    muliplyMatrix(shm->matQ[k].mat, shm->matQ[0].mat, shm->matQ[1].mat, 1) ;   
                    cout << "worker work " << work_no << " done " << endl ; 
                    cout << "status " << st1 << endl ;      
                    sem_post(&(shm->full));	
                    (shm->matQ[0].status)++ ; 
                    (shm->matQ[1].status)++ ; 
                    sem_post(&(shm->full));	
                }
                else if(st1 == 2){
                    // multiply with status = 1
                    int k = shm->ind ;
                    if(k == MAX_QUEUE_SIZE || k < 2){
                        cout << "ERROR in worker calculation side and size" << endl ; 
                        sem_post(&(shm->mutex)) ;
                        break ; 
                    }
                    shm->ind = k ;

                    muliplyMatrix(shm->matQ[k].mat, shm->matQ[0].mat, shm->matQ[1].mat, 2) ;   
                    cout << "worker work " << work_no << " done " << endl ; 
                    cout << "status " << st1 << endl ;      
                    sem_post(&(shm->full));	
                    (shm->matQ[0].status)++ ; 
                    (shm->matQ[1].status)++ ; 
                    sem_post(&(shm->full));	


                }
                else if(st1 == 3){
                    // multiply with status = 1
                    int k = shm->ind ;
                    if(k == MAX_QUEUE_SIZE || k < 2){
                        cout << "ERROR in worker calculation side and size" << endl ; 
                        sem_post(&(shm->mutex)) ;
                        break ; 
                    }
                    shm->ind = k ;

                    muliplyMatrix(shm->matQ[k].mat, shm->matQ[0].mat, shm->matQ[1].mat, 3) ;   
                    cout << "worker work " << work_no << " done " << endl ; 
                    cout << "status " << st1 << endl ;      
                    sem_post(&(shm->full));	
                    (shm->matQ[0].status)++ ; 
                    (shm->matQ[1].status)++ ; 
                    sem_post(&(shm->full));	
                }
                else if(st1 == 4){
                    // multiply with status = 1
                    int k = shm->ind ;
                    if(k == MAX_QUEUE_SIZE || k < 2){
                        cout << "ERROR in worker calculation side and size" << endl ; 
                        sem_post(&(shm->mutex)) ;
                        break ; 
                    }
                    shm->ind = k ;

                    muliplyMatrix(shm->matQ[k].mat, shm->matQ[0].mat, shm->matQ[1].mat, 4) ;   
                    cout << "worker work " << work_no << " done " << endl ; 
                    cout << "status " << st1 << endl ;      
                    sem_post(&(shm->full));	
                    (shm->matQ[0].status)++ ; 
                    (shm->matQ[1].status)++ ; 
                    sem_post(&(shm->full));	
                }
                else if(st1 == 5){
                    // multiply with status = 1
                    int k = shm->ind ;
                    if(k == MAX_QUEUE_SIZE || k < 2){
                        cout << "ERROR in worker calculation side and size" << endl ; 
                        sem_post(&(shm->mutex)) ;
                        break ; 
                    }
                    shm->ind = k ;

                    muliplyMatrix(shm->matQ[k].mat, shm->matQ[0].mat, shm->matQ[1].mat, 5) ;   
                    cout << "worker work " << work_no << " done " << endl ; 
                    cout << "status " << st1 << endl ;      
                    sem_post(&(shm->full));	
                    (shm->matQ[0].status)++ ; 
                    (shm->matQ[1].status)++ ; 
                    sem_post(&(shm->full));

                }
                else if(st1 == 6){
                    // multiply with status = 1
                    int k = shm->ind ;
                    if(k == MAX_QUEUE_SIZE || k < 2){
                        cout << "ERROR in worker calculation side and size" << endl ; 
                        sem_post(&(shm->mutex)) ;
                        break ; 
                    }
                    shm->ind = k ;

                    muliplyMatrix(shm->matQ[k].mat, shm->matQ[0].mat, shm->matQ[1].mat, 6) ;   
                    cout << "worker work " << work_no << " done " << endl ; 
                    cout << "status " << st1 << endl ;      
                    sem_post(&(shm->full));	
                    (shm->matQ[0].status)++ ; 
                    (shm->matQ[1].status)++ ; 
                    sem_post(&(shm->full));

                }
                else if(st1 == 7){
                    // multiply with status = 1
                    int k = shm->ind ;
                    if(k == MAX_QUEUE_SIZE || k < 2){
                        cout << "ERROR in worker calculation side and size" << endl ; 
                        sem_post(&(shm->mutex)) ;
                        break ; 
                    }
                    shm->ind = k ;

                    muliplyMatrix(shm->matQ[k].mat, shm->matQ[0].mat, shm->matQ[1].mat, 7) ;   
                    cout << "worker work " << work_no << " done " << endl ; 
                    cout << "status " << st1 << endl ;      
                    sem_post(&(shm->full));	
                    (shm->matQ[0].status)++ ; 
                    (shm->matQ[1].status)++ ; 
                    sem_post(&(shm->full));
                }
                else if(st1 == 8){
                    int k = shm->ind ;
                    shm->matQ[shm->ind].status = 0 ; 
                    Matrix m1 = deleteMatrix(shm) ;
                    sem_post(&(shm->full));
                    Matrix m2 = deleteMatrix(shm) ; 
                    sem_post(&(shm->full));

                    shm->ind = 0 ; 
                    cout << "done for two matrices" << endl ; 
                    sem_post(&(shm->full));

                }
                else{
                    
                    cout << "Something is wrong" << endl ;
                    sem_post(&(shm->mutex)) ;
                    break ; 
                }
                sem_post(&(shm->mutex)) ;
            }
        }
        else{
            sem_post(&(shm->mutex)) ;
            break ; 
        }
        sem_post(&(shm->mutex)) ;
    }

}

// *************************************************************************

int main(){
    srand(time(0)) ; 

    int NP, NW, num_matrix ; 
    cout << "Enter number of Producers" << endl ; 
    cin >> NP ; 
    cout << "Enter number of workers" << endl ; 
    cin >> NW ; 
    cout << "Enter number of Matrixes to multiply" << endl ; 
    cin >> num_matrix ; 

    pid_t produc[NP] , work[NW] ; 
    key_t key = 1232 ; 
    int shmid = shmget(key, sizeof(SharedMemory), 0666|IPC_CREAT) ; 

    if(shmid<0){
		printf("Error in creating shared memory. Exitting..\n");
		exit(1);
	}
    SharedMemory* shm = (SharedMemory*)shmat(shmid, NULL, 0) ;
    init_SHM(shm, num_matrix) ; 
    time_t start = time(0) ; 
    pid_t pid ;

    for(int i = 1 ; i <= NP ; i++){

        pid = fork() ; 
        if(pid < 0){
            cout << "Error in creating producer process" << endl ; 
            exit(1) ; 
        }
        else if(pid == 0){
            srand(time(0) + i) ; 
            // int prod_pid = getpid() ; 
            cout << "process " << i << endl ; 
            producer(shm, i, getpid(), num_matrix) ; 
            return 0 ; 
        }
        else{
            produc[i-1] = pid ; 
        }
    }

    for(int i = 1 ; i <= NW ; i++){
        pid = fork() ; 

        if(pid < 0){
            cout << "Error in creating worker process" << endl ; 
            exit(1) ; 
        }
        else if(pid == 0){
            srand(time(0) + NP+ i) ; 
             
            // int cons_pid = getpid() ; // status
            worker(shm, i, getpid(), num_matrix) ; 

            return 0 ; 
        }
        else{
            work[i-1] = pid ; 
        }
    }


    while(1){
		// acquire lock so that while checking, state change not possible
		sem_wait(&(shm->mutex));
		// shm->computed ensures that consumer gets killed only after it has computed/slept
		// if(shm->job_created>=num_matrix && shm->job_completed>=num_matrix && shm->computed>=num_matrix)
		if(shm->job_created>=num_matrix)
        {
			time_t end = time(0);
			int time_taken = end-start;
			printf("Time taken to run %d jobs= %d seconds\n",num_matrix,time_taken);
			// kill all child processes
			for(int i=0;i<NP;i++)
				kill(produc[i],SIGTERM);
			for(int i=0;i<NW;i++)
				kill(work[i],SIGTERM);
			sem_post(&(shm->mutex));
			break;		
		}
		sem_post(&(shm->mutex));
	}

    sem_destroy(&(shm->mutex));
    shmdt(shm) ; 
    shmctl(shmid, IPC_RMID, nullptr) ; 
    return 0 ; 
}