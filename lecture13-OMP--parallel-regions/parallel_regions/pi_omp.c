

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#ìnclude <omp.h>


#define CPU_TIME_W ({ struct timespec ts; (clock_gettime( CLOCK_REALTIME, &ts ), \
                                           (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9); })
#define CPU_TIME_T ({ struct timespec myts; (clock_gettime( CLOCK_THREAD_CPUTIME_ID, &myts ), \
                                             (double)myts.tv_sec + (double)myts.tv_nsec * 1e-9); })
#define CPU_TIME_P ({ struct timespec myts; (clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &myts ), \
                                             (double)myts.tv_sec + (double)myts.tv_nsec * 1e-9); })


typedef unsigned int uint;


int main ( int argc, char **argv )
{

  uint Nshots       = (argc>1 ? (uint)atoi(*(argv+1)) : 1000000 );
  uint valid_points = 0;
  
  srand48(time(NULL));
  double timing = CPU_TIME_P;

  
  #pragma omp parallel
  {
    int                myid    = omp_get_thread_num();
    unsigned short int seed[3] = { myid, myid & 123, myid*11 };
    uint               my_validpoints = 0;

    double mytiming = CPU_TIME_T;
    for ( uint i = 0; i < Nshots; i++ )
      {
	double x = erand48(seed);
	double y = erand48(seed);
	
	// 2) check whether they are in the circle of radius 1
	// 3) increase the local counter my_validpoints
      }
    mytiming = CPU_TIME_T - mytiming;

    // 4) get the total counter valid_points
    
    printf("thread %d has run for %g sec\n", mytiming );
  }
  
  timing = CPU_TIME_P - timing;
    
  printf("value of pi greek is: %g\n"
	 "elapsed time is %g sec\n",
	 valid_points/Nshots * 4, timing);
  

  return 0;
}
