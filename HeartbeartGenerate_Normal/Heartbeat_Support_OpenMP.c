#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include"Heartbeat_Support_OpenMP.h"
#include <pthread.h>

heartbeat_profile_t heart[NUM_THREADS];
pthread_mutex_t __mutex;

static inline heartbeat_log_t* __L_Heartbeat_Alloc_Log(int64_t size) {
     
  heartbeat_log_t* p = NULL;
  
  p = malloc(size*sizeof(heartbeat_log_t));          //assign number of "buffer size" the type of heartbeat_log memory;

  return p;                                          //return the first address of the memory;
}

static inline heartbeat_memory_t* __L_Heartbeat_Alloc_Memory(int64_t size) {
     
  heartbeat_memory_t* p = NULL;
  
  p = malloc(size*sizeof(heartbeat_memory_t));   //assign number of "buffer size" the type of heartbeat_memory_t memory;

  return p;                                          //return the first address of the memory;
}

static void __L_Heartbeat_flush_buffer(heartbeat_profile_t volatile * hb) {
  long i;
  long numb = hb->buffer_to_file;                               // total number of buffer chunks;`
  if(hb->text_file != NULL) {
   //      fseek(hb->text_file,0L,SEEK_END);
    for(i = 0; i < numb; i++) {
      fprintf(hb->text_file,                                    // write info in memory to log;
	      "%ld    %d   %d   %lld   %lf   \n", 
	       hb->log[i].beat,                                     // write ;the current heartbeat info, beat, to log;
	       hb->log[i].threadnum,                                // write the current heartbeat info, tag, to log;
           hb->log[i].loopnum,
           (long long int) hb->log[i].timestamp,                // write the current heartbeat info, timestamp, to log;
	       hb->log[i].heartbeat_rate);                          // write the current heartbeat info, heartbeat rate, to log;
    }   
    fflush(hb->text_file);                                      // flush the input memory
  }
}

void __L_Heartbeat_OpenMP_End(heartbeat_profile_t * hb){                                      

    if(hb->log != NULL){                                                              //free the log memory;
       free(hb->log);          
   }
   if(hb->memory !=NULL){
       free(hb->memory);
   }
    if(hb->text_file != NULL)                                                        // close the text_file;
    fclose(hb->text_file);
}

int L_Heartbeat_OpenMP_Init(){
    int num_thread,rc;
    char pthread_ID[300];          // save the name of thread number;
    num_thread=omp_get_num_threads();
    printf("omp_get_num_threads=%d\n", num_thread);
    int i=0;
    int64_t size = 50;      
    for(i=0;i<NUM_THREADS;i++){
      printf("i=%d",i);
    heart[i].tid = i;  //get the thread number;
    heart[i].time_temp=0;
    heart[i].mean_freq=0;
    heart[i].log = __L_Heartbeat_Alloc_Log(size);  //assign a memory space for heartbeat;
    heart[i].memory= __L_Heartbeat_Alloc_Memory(1000);
    if(heart[i].log == NULL||heart[i].memory==NULL){
    rc = 2;                      //log name missing;
    }
    heart[i].acc_index=0;
    sprintf(pthread_ID, "2020-0%d",(unsigned)heart[i].tid);   //write the thread name to the "pthread_ID";
    if (heart[i].text_file = fopen(pthread_ID, "w")){
    fprintf(heart[i].text_file, "Beat    ThreadNum   iter    Timestamp    Heartbeat Rate \n" ); //write the name to the sheet header;
    }
    else{
    heart[i].text_file = NULL;                                     //open file failed;
    rc=1;
    }
    heart[i].first_timestamp = heart[i].last_timestamp = 0;        //initialize the timestamp for the first time;
    heart[i].current_index = 0;                                    //initialize the current_index for the first time;
    heart[i].counter = 0;                                          //initialize the counter for the first time;
    heart[i].buffer_index = 0;                                     //initialize the buffer_index for the first time;
    heart[i].buffer_to_file = size;                                //initialize the buffer_depth for the first time;
   }    
    return rc;
}

int L_Heartbeat_OpenMP_Generate(int threadnum,int loopnum, int iter){
    struct timespec time_info;                   // define time_info for save current time info;
    int64_t time;                                // save the time as time point (nanosecond);
    int index;   
    int counter;
    int k;
    k=omp_get_thread_num(); 
    clock_gettime(CLOCK_REALTIME, &time_info );  //get the current time;
    time = ( (int64_t) time_info.tv_sec * 1000000000 + (int64_t) time_info.tv_nsec ); // save the time as time point (nanosecond);                                               
    heart[k].last_timestamp = time;   
    if(heart[k].first_timestamp == 0) {
    index = 0;                                                                      // deal with a heartbeat for the first timestamp;
    heart[k].first_timestamp = time;                                                     // timestamp for the first time;
    heart[k].last_timestamp  = time;                                                     // timestamp for the previous time;
    heart[k].log[0].beat = 0;                                                            // write a beat as log for the first timestamp;
    heart[k].log[0].threadnum= threadnum;                                                           // write a tag as log for the first timestamp;
    heart[k].log[0].loopnum= loopnum;
    heart[k].log[0].timestamp = time;                                                    // write a timestamp as log for the first timestamp;
    heart[k].log[0].heartbeat_rate = 0;                                                  // write a heartbeat rate as log for the first timestamp;
    heart[k].memory[0].timestamp=time;
    heart[k].memory[0].heartbeat_rate=0;
    heart[k].memory[0].tid=heart[k].tid;
    heart[k].counter++;                                                                  // heartbeats counter;
    //heart[k].buffer_index++;                                                             // buffer index;
    }
    else {                                                                            // redefine number of heartbeats; 
       if((iter)% (HEART_FREQUENCY*1)==0){                                             // cycle is total number of heartbeats;
           heart[k].last_timestamp = time;                                                      // previous the timestamp;
           double heartbeat_rate = (((double) heart[k].counter) / ((double) (time - heart[k].first_timestamp)))*1000000000.0;  //function for calculate the heartbeat rate;
           index =  heart[k].buffer_index;  
           counter = heart[k].counter;
           heart[k].log[index].beat = heart[k].counter;                                              // prepared to save the current heartbeat info, beat, to log.
           heart[k].log[index].threadnum = threadnum;                                                       // prepared to save the current heartbeat info, tag, to log.
           heart[k].log[index].loopnum=iter;
           heart[k].log[index].timestamp = (double)(time - heart[k].first_timestamp);  
    //printf("线程号:%d---i:%d---j:%d---iter:%d---时间差:%lf\n",k,i,j,iter, (double)(time - heart[k].first_timestamp));
                                                  // prepared to save the current heartbeat info, timestamp, to log.
           heart[k].log[index].heartbeat_rate = heartbeat_rate;                                 // prepared to save the current heartbeat info, heartbeat rate, to log.
           heart[k].memory[counter].timestamp= time;
           heart[k].memory[counter].heartbeat_rate = heartbeat_rate;
           heart[k].memory[counter].tid=heart[k].tid;
           heart[k].buffer_index++;                                                             
           heart[k].counter++;
           if(heart[k].buffer_index%heart[k].buffer_to_file == 0){                                    //write the log to the file                         
	            if(heart[k].text_file != NULL)
	            __L_Heartbeat_flush_buffer(&heart[k]);    
	       heart[k].buffer_index = 0;
            }
           }
    if(heart[k].counter==1000)
    {    
	heart[k].counter=1;
	heart[k].first_timestamp = time;
    }                               
}
return 0;
}

void L_Heartbeat_OpenMP_Finished(){
    int i;
    for(i=0;i<NUM_THREADS;i++){
    __L_Heartbeat_OpenMP_End(&heart[i]);
    }
}