#ifndef HEARTBEAT_SUPPORT_OPENMP_H
#define	HEARTBEAT_SUPPORT_OPENMP_H
#define NUM_THREADS 4
#define HEART_FREQUENCY 1000

typedef struct {
  long beat;                 // Heartbeat 
  int  threadnum;            // used for recongnizing each beat
  int  loopnum;
  long timestamp;            // used for record each beat time
  double heartbeat_rate;     // refeclt the beat frequency
} heartbeat_log_t;           // struct used for record each beat data

typedef struct {
  long index;
  pthread_t tid; 
  double heartbeat_real_value;
  double heartbeat_rate;
  long timestamp;
} heartbeat_memory_t;

typedef struct {
  pthread_t tid;             // used for multi-threads as saving a thread number
  long counter;               // beat counter
  long buffer_to_file;       // at one time write how many buffer size to file
  long buffer_index;         // buffer index
  long fcounter;
  long fbuffer_to_file;
  long fbuffer_index;
  int64_t first_timestamp;   // timestamp for last time
  int64_t last_timestamp;    // timestamp for current time
  int64_t acc_timestamp_first;
  int64_t acc_timestamp_last;
  int64_t time_temp;
  int acc_index;
  long current_index;        // current index
  int64_t max;                  // max number of heartbeats in a group
  long interval;             // interval between two beats (nanosecond)
  int frequency;             // input 2, 4, 5, or ... n denotes total number/n times
  int64_t mean_freq;
  int cycle;                 // used for total number of loop times
  heartbeat_log_t* log;      // pointer for log
  heartbeat_memory_t* memory; //pointer for memory
  FILE*  text_file;           // pointer to file saving heartbeat record.
  pthread_mutex_t mutex;     // mutex for some memory operations. 
} heartbeat_profile_t;       // heartbeat profile.

#ifdef	__cplusplus
extern "C" {
#endif

int L_Heartbeat_OpenMP_Init();

int L_Heartbeat_OpenMP_Generate(int threadnum,int loopnum, int iter);
              
void L_Heartbeat_OpenMP_Finished();

#ifdef	__cplusplus
}
#endif

#endif	/* HEARTBEAT_SUPPORT_OPENMP_H */

