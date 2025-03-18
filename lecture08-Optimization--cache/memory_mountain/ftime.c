/* Compute time used by a function f that takes two integer args */
#include <stdlib.h>
#include <stdio.h>

#include "clock.h"
#include "ftime.h"


void     add_sample(double, double *, int, int);
int      has_converged(double *, int, double);


//
// Add new sample
//
void add_sample(double value, double *values, int current_count, int max_count)
{
  int pos = 0;
  
  if (current_count < max_count) {
    pos = current_count-1;
    values[pos] = value;
  } else if (value < values[max_count-1]) {
    pos = max_count - 1;
    values[pos] = value;
  }

  /* Insertion sort */
  while ( (pos > 1) &&
	  (values[pos-1] > values[pos]) )
    {
      double temp = values[pos-1];
      values[pos-1] = values[pos];
      values[pos] = temp;
      pos--;
    }

  return;
}


/* What is relative error for kth smallest sample */
double err(double *values, int k, int max_count)
{
  if (k < max_count)
    return 1.0;
  return (values[k-1] - values[0])/values[0];
}

/* Have k minimum measurements converged within epsilon? */
int has_converged(double *values, int max_count, double epsilon)
{
  return ((1 + epsilon)*values[0] >= values[max_count-1]);
}

static int sink;
void clear_cache( void )
{
 #define CACHELINE 64
  int stride = ( CACHELINE  / (sizeof(int)) );

 #define MEMSIZE (1<<20)
  int size = ( MEMSIZE / sizeof(int) );

  int *stuff = (int*)malloc( MEMSIZE );

  int x = sink; 
  for (int i = 0; i < size; i += stride)
    x += stuff[i];
  sink = x;
  
  free( stuff );
  return;
}


double ftime (test_funct f,        // the test function to be used
	      int *param0,         // the pointer to data
	      int param1,          // how many elements in the data array
	      int param2,          // he stride to be used
	      double *values,      // the array to store the timings
	      int max_count,       // how many best timings
	      int max_samples,     // how many samples
	      double epsilon )	   // the convergence threshold
{
 #define WARM_CYCLES 2
  double  overhead  = get_clock_overhead(NULL);
  int     nsamples  = 0;
  int     stop      = 0;
  
  do {
    
    nsamples++;
    clear_cache();
    
    double timing;
    for ( int jj = 0; jj < WARM_CYCLES; jj++ )
      {
	
	double time_start = CPU_TIME;
	int res = f(param0, param1, param2);   // the first WARM_CYCLES-1 iterations warm
	                                       // cache and throttle core's frequency
	timing = (CPU_TIME - time_start) - overhead;
      }

    add_sample(timing, values, nsamples, max_count);

    if ( nsamples >= max_count )
      stop = has_converged( values, max_count, epsilon );
    
  } while ( (nsamples < max_samples) && !stop );

  double result = 0;

  // take the average of the best measures as our
  // estimate
  // note: we discard the very best result,
  // considering it as a glitch
  //
  for ( int i = 1; i < max_count; i++ )
    result += values[i];
  result /= (max_count-1);
  
  return result;
}
