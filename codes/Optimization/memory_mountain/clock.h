/* Routines for using cycle counter */


#include <sys/times.h>
#include <time.h>

#define CPU_TIME ({struct  timespec ts; clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ts ),	\
                                          (double)ts.tv_sec +           \
                                          (double)ts.tv_nsec * 1e-9;})


/* Start the counter */
void start_counter();

/* Get # cycles since counter started */
double get_counter();

double get_clock_overhead ( double * );
