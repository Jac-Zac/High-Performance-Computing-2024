#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CPU_TIME_W                                                             \
  ({                                                                           \
    struct timespec ts;                                                        \
    clock_gettime(CLOCK_REALTIME, &ts);                                        \
    (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;                             \
  })

#define CPU_TIME_T                                                             \
  ({                                                                           \
    struct timespec ts;                                                        \
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);                               \
    (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;                             \
  })

#define CPU_TIME_P                                                             \
  ({                                                                           \
    struct timespec ts;                                                        \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);                              \
    (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;                             \
  })

typedef unsigned int uint;

int main(int argc, char **argv) {
  uint Nshots = (argc > 1 ? (uint)atoi(argv[1]) : 1000000);
  uint valid_points = 0;

  double timing = CPU_TIME_P;

  // Make a reduction
#pragma omp parallel reduction(+ : valid_points)
  {
    int myid = omp_get_thread_num();
    unsigned short seed[3] = {(unsigned short)(time(NULL) + myid),
                              (unsigned short)(myid * 19),
                              (unsigned short)(myid * 13 + 7)};

    double mytiming = CPU_TIME_T;

    // Distribute loop among threads automatically
#pragma omp for
    for (uint i = 0; i < Nshots; i++) {
      double x = erand48(seed);
      double y = erand48(seed);
      valid_points += ((x * x + y * y) <= 1.0);
    }

    mytiming = CPU_TIME_T - mytiming;
    printf("Thread %d ran for %g sec\n", myid, mytiming);
  }

  timing = CPU_TIME_P - timing;

  printf("Estimated value of Pi: %g\n"
         "Total elapsed time: %g sec\n",
         (double)valid_points / Nshots * 4.0, timing);

  return 0;
}
