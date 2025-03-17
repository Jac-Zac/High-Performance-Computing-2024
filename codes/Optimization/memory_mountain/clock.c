
#include <math.h>
#include "clock.h"


/* Start the counter */
void start_counter();

/* Get # cycles since counter started */
double get_counter();


double get_overhead ( double *);


double get_clock_overhead ( double *do_not_optimize )
{
  double time_all   = 0.785398;
  double time_start = CPU_TIME;

  // induce a throttle-up of the CPU frequency
  for ( int j = 0; j < 1000; j++ ) 
    time_all += exp(sin(time_all));

  // measure how much CPU_TIME takes
  for ( int j = 0; j < 100; j++ )
    time_all += CPU_TIME;
  
  if ( do_not_optimize != NULL )
    *do_not_optimize = time_all;
  return (CPU_TIME - time_start) / 100;  
}
