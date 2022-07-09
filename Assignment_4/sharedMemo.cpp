// Group 20
// Assignment 4

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
#include <pthread.h>
#include <mutex>

using namespace std ; 

#define db(...) __f(#__VA_ARGS__, __VA_ARGS__)
 
template <typename Arg1>
void __f(const char* name, Arg1&& arg1) { cout << name << " : " << arg1 << '\n'; }
template <typename Arg1, typename... Args>
void __f(const char* names, Arg1&& arg1, Args&&... args) {
    const char* comma = strchr(names + 1, ',');
    cout.write(names, comma - names) << " : " << arg1 << " | "; __f(comma + 1, args...);
}

#define MAX_JOB_TIME 250
#define MAX_JOB_SIZE 500
#define MAX_JOB_ID 1e8 

/************************************************************************************************/
int get_random(int start, int end) ; 

void* runnerP(void* arg) ; 

void* runnerCons(void* arg) ; 

void init_shm() ; 

void init_SHM_tree(int num) ; 
/************************************************************************************************/

typedef struct Node{

    int job_id ; 
    int comp_time ;
    
    // -1 for not yet started 0 for completed and 1 for done 
    int status ; 
    pthread_mutex_t lock ; 
    
    // count for children completed

    Node* dependent_job[MAX_JOB_SIZE] ;      
    Node* parent ; 
    int count_dependent , complete_dependent ; 
    
    // make a constructor too

    Node(): status(-1), job_id(get_random(0, MAX_JOB_ID)), comp_time(get_random(1, MAX_JOB_TIME)){
        pthread_mutexattr_t attr ; 
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&lock, &attr) ; 
        count_dependent = 0 ; 
        complete_dependent = 0 ; 
        parent = NULL ; 
    }

    int check_dependency(){
        // check dependency by dfs
        for(int i = 0 ; i < count_dependent ; i++){
            Node* t = dependent_job[i] ; 
            pthread_mutex_lock(&t->lock) ; 
            if(t->status != 1){
                // printf("Job %d has still an unfree child %d\n", job_id, t->job_id) ; 
                pthread_mutex_unlock(&t->lock) ; 
                return i ; 
            }
            pthread_mutex_unlock(&t->lock) ; 
        }
        return -1 ;
    }

    void print_status(){
        printf("Status           ") ; 
        fflush(stdout);
        if(status == -1){
            printf("NOT STARTED\n") ;
            fflush(stdout); 
        }
        else if(status == 0){
            printf("ONGOING\n") ;
            fflush(stdout);
        }
        else{
            printf("COMPLETED\n") ;
            fflush(stdout); 
        }
        fflush(stdout);
    }

    void print_job_details(int pid, int flag = 0){
        printf("Job details are:\n") ;
        fflush(stdout); 
        printf("JOB ID is        %d\n", job_id) ; 
        fflush(stdout);
        if(flag == 0)   print_status() ;
        else    printf("Status : Currently a child is added\n") ;  
        fflush(stdout);
        printf("Completion time  %d\n", comp_time) ;
        fflush(stdout); 
        printf("Job is handled by %d\n", pid) ;
        fflush(stdout); 
    }

    void run_job(int pid){
        
        pthread_mutex_lock(&lock) ; 
        status = 0 ; 
        print_job_details(pid, 0) ; 
        pthread_mutex_unlock(&lock) ;
        
        usleep(comp_time*1000) ;

        pthread_mutex_lock(&lock) ; 
        status = 1 ; 
        print_job_details(pid, 0) ; 
        pthread_mutex_unlock(&lock) ; 
    }

} Node ;

typedef struct share{

    int count ;
    int size ;  
    Node* tree[10*MAX_JOB_SIZE] ; 
    int job_added, job_finished ; 
    pthread_mutex_t shm_lock ; 

} ShareMem ; 

ShareMem* shm ; 

/************************************************************************************************/
void init_shm(){
    shm->count = 0 ;
    shm->size = 10*MAX_JOB_SIZE ;  

    pthread_mutexattr_t attr ; 
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&shm->shm_lock, &attr) ; 
    shm->job_added = 0 ; 
    shm->job_finished = 0 ; 
}

void init_SHM_tree(int num){
    for(int i = 0 ; i < num ; i++){
        Node* temp = new Node ; 
        shm->tree[shm->count++] = temp ; 
        if(shm->count > 1){
            int p_id = get_random(0, shm->count - 1) ;
            Node* parent_job = shm->tree[p_id] ; 
            parent_job->dependent_job[parent_job->count_dependent++] = temp ;
            // parent_job->print_job_details(-1, 1) ;  
        }   
    }
}

int get_random(int start, int end){ 
    int ans = rand()%(end - start + 1) + start ; 
    return ans ; 
}

void* runnerP(void* arg){
    int pid ; 
    pid =  *reinterpret_cast<int*>(arg);
    int t = get_random(10, 20)*1000 ;  // seconds to miliseconds 

    while(t >= 0){
        int sleep_time = get_random(200, 500) ;  // mili seconds 
        Node* temp = new Node ; 

        pthread_mutex_lock(&(shm->shm_lock)) ; 

        if(shm->count == shm->size){
            pthread_mutex_unlock(&(shm->shm_lock)) ; 
            break ; 
        }
        int p_lim = shm->count ; 
        shm->tree[shm->count++] = temp ; 
        // db(shm->count) ; 
        pthread_mutex_unlock(&(shm->shm_lock)) ; 

        // // pthread_mutex_lock(&(shm->shm_lock)) ;
        // Node* parent_job = shm->tree[p_id] ; 
        // pthread_mutex_lock(&(parent_job->lock)) ; 
        
        // // problem when number of parent job valid left is very very low
        // // this loop will run many times
        // if(parent_job->status != -1){
        //     pthread_mutex_unlock(&(parent_job->lock)) ;
        //     continue ;     
        // }
        // parent_job->dependent_job[parent_job->count_dependent++] = temp ; 
        // // parent_job->print_job_details(1) ; 
        // db("Print here", parent_job->job_id, parent_job->comp_time) ; 
        // pthread_mutex_unlock(&(parent_job->lock)) ;

        for(int i = 0 ; i < p_lim ; i++){
            pthread_mutex_lock(&(shm->tree[i]->lock)) ; 
            if(shm->tree[i]->status != -1){
                pthread_mutex_unlock(&(shm->tree[i]->lock)) ; 
                continue ; 
            }
            else{
                Node* parent_job = shm->tree[i] ;
                parent_job->dependent_job[parent_job->count_dependent++] = temp ;
                parent_job->print_job_details(pid, 1) ; 
                // db("Print here", parent_job->job_id, parent_job->comp_time) ;
                pthread_mutex_unlock(&(parent_job->lock)) ;
                break ; 
            }
        }
        // pthread_mutex_lock(&(shm->shm_lock)) ; 
        t -= (sleep_time) ; 
    }
    printf("Producer Exited %d\n", pid) ;  
    fflush(stdout); 
    pthread_exit(NULL) ; 
    // pthread exit code
}

void* runnerCons(void* arg){
    int pid ; 
    pid =  *reinterpret_cast<int*>(arg);
    int c1 = 1 , c2 = 1, c3 = 1 ; 
    // db("worker", pid) ;  
    while(true){

        pthread_mutex_lock(&(shm->shm_lock)) ;
        // db("worker3", c3, pid); 
        int k = shm->count ; 
        
        if(shm->job_finished == shm->count){
            pthread_mutex_unlock(&(shm->shm_lock)) ; 
            break ; 
        }
        // db("Fbreak") ; 
        pthread_mutex_unlock(&(shm->shm_lock)) ; 
        int flag = 0 ; 

        for(int i = k-1 ; i >= 0 ; i--){
            pthread_mutex_lock(&(shm->shm_lock)) ;
            if(shm->job_finished == shm->count){
                flag = 2 ; 
                break ; 
            }
            pthread_mutex_unlock(&(shm->shm_lock)) ; 
            // db("Fb") ; 

            pthread_mutex_lock(&(shm->tree[i]->lock)) ; 

            if(shm->tree[i]->status != -1){
                // db("FFFFp") ; 
                pthread_mutex_unlock(&(shm->tree[i]->lock)) ;    
                continue ;  
            }
            else{
                int c = shm->tree[i]->check_dependency() ; 
                // Job is ready to consumed
                if(c == -1){ 
                    // db("FFF") ; 
                    pthread_mutex_unlock(&(shm->tree[i]->lock)) ; 
                    shm->tree[i]->run_job(pid) ;
                    pthread_mutex_lock(&(shm->shm_lock)) ;
                    shm->job_finished++ ;
                    // db(shm->job_finished) ;  
                    pthread_mutex_unlock(&(shm->shm_lock)) ; 
                    flag = 2 ; 
                    break ;  
                }   
                else{
                    // db("FFP") ; 
                    pthread_mutex_unlock(&(shm->tree[i]->lock)) ;
                    continue ;  
                }
            }
        }
        // db("worker end inside while", pid) ; 
        // db("why") ; 
        if(flag == 2){
            break ; 
        }

    }
    printf("Consumer Exited %d\n", pid) ; 
    fflush(stdout); 
    // db("consumer") ; 
    pthread_exit(NULL) ; 
}

/************************************************************************************************/

int main(){

    srand(time(0)) ; 

    int P, y ; 
    printf("Enter the number of Producer\n") ; 
    cin >> P ; 

    printf("Enter the number of consumer threads\n") ; 
    cin >> y ; 

    key_t key = 1232 ; 
    int shmid = shmget(key, sizeof(ShareMem), 0666|IPC_CREAT) ;

    if(shmid<0){
		printf("Error in creating shared memory. Exitting..\n");
		exit(1);
	}

    shm = (ShareMem*)shmat(shmid, NULL, 0) ;
    init_shm() ; 
    init_SHM_tree(get_random(300, 500)) ; 

    // Process A
    pthread_t producer_id[P] ; 

    for(int i = 0 ; i < P ; i++){
        int p = pthread_create(&(producer_id[i]), NULL, runnerP, (void *)&i) ; 
        if(p != 0){
            printf("Error in thread creation\n") ; 
            return 0 ;
        }
    }

    // Now forking a process B
    pid_t child = fork() ; 

    if(child < 0){
        printf("some error in forking child\n") ; 
        exit(EXIT_FAILURE) ; 
    }
    else if(child == 0){
        pthread_t consumer_id[y] ; 

        for(int i = 0 ; i < y ; i++){
            int p = pthread_create(&consumer_id[i], NULL, runnerCons, (void *)&i ) ; 
            if(p != 0){
                printf("Error in thread creation\n") ; 
                return 0 ;
            }
        }

        for(int i = 0 ; i < y ; i++){
            pthread_join(consumer_id[i], NULL) ; 
        }

        exit(EXIT_SUCCESS) ; 
    }
    else{
        
        for(int i = 0 ; i < P ; i++){
            pthread_join(producer_id[i], NULL) ; 
        }
        
        // while(wait(NULL) < 0)
        //     ;
        wait(NULL) ; 

    }

}
