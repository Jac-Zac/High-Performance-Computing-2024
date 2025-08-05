#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Macro for timing
#define CPU_TIME_P                                                             \
  ({                                                                           \
    struct timespec myts;                                                      \
    (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &myts),                           \
     (double)myts.tv_sec + (double)myts.tv_nsec * 1e-9);                       \
  })

typedef unsigned int uint;

int main(int argc, char **argv) {

  uint Nshots = (argc > 1 ? (uint)atoi(*(argv + 1)) : 1000000);
  uint valid_points = 0;

  // define the random number generator with a seed given by the time
  srand48(time(NULL));

  double timing = CPU_TIME_P;
  for (uint i = 0; i < Nshots; i++) {
    // 1) generate x and y by rand48()
    double x = drand48();
    double y = drand48();
    // 2) check whether they are in the circle of radius 1
    // 3) increase the counter
    // The two steps are done in a branch less way
    valid_points += (x * x + y * y <= 1);
  }
  timing = CPU_TIME_P - timing;

  printf("value of pi greek is: %g\n"
         "elapsed time is %g sec\n",
         (double)valid_points / Nshots * 4, timing);

  return 0;
}
