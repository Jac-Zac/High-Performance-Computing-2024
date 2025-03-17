

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "ftime.h" /* measurement routines */
#include "clock.h" /* routines to access the cycle counter */

#define MINBYTES (1 << 14)  /* First working set size */
#define MAXBYTES (1 << 27)  /* Last working set size */
#define MAXSTRIDE 15        /* Stride x8 bytes */
#define MAXELEMS MAXBYTES/sizeof(int) 



void   init_data ( int *, int );
int    test      ( int *, int, int );
double run       ( int, int, double*, int, int, double );


int main( int argc, char **argv )
{
  int max_count   = ( argc > 1 ? atoi(*(argv+1)) : 8);
  int max_samples = ( argc > 2 ? atoi(*(argv+2)) : 500);
  double epsilon  = ( argc > 3 ? atof(*(argv+3)) : 0.001); 
    
  double *values = (double*)malloc( max_count*sizeof(double) );
  int    *data   = (int*)aligned_alloc( 64, MAXBYTES );
  
  init_data(data, MAXELEMS); /* Initialize each element in data */
  
    
  printf("# Memory mountain (MB/sec)\n");
  printf("--\t");
  for (int stride = 1; stride <= MAXSTRIDE; stride++)
    printf("%d\t", stride);
  printf("\n");

  //
  // run mountain
  //
  for (int size = MAXBYTES; size >= MINBYTES; size >>= 1)
    {
      int log2size_kb = (int)(log2((double)size / 1024.0));
      printf("%d\t", log2size_kb);
      
      int n_elements = size / sizeof(int);
      
      for (int stride = 1; stride <= MAXSTRIDE; stride++)
	{
	  
	  double timing = ftime( test, data, n_elements, stride, values, max_count, max_samples, epsilon);
	  printf("%.0f\t", (size / (1024.0*1024.0) / stride) / timing);
	}
      
      printf("\n");
    }

  free ( values );
  free ( data );
  return 0;
}


/* init_data - initializes the array */
void init_data(int *data, int N)
{
    for (int i = 0; i < N; i++)
      data[i] = i;
}

/* $begin mountainfuns */
/* test - Iterate over first "elems" elements of array "data" with
 *        stride of "stride", using 4x4 loop unrolling.
 */


int test(int *data, int elems, int stride)

{

  int i, sx2 = stride*2, sx3 = stride*3, sx4 = stride*4;
  int acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;
  int length = elems;
  int limit = length - sx4;
  
    /* Combine 4 elements at a time */
    for (i = 0; i < limit; i += sx4) {
      acc0 = acc0 + data[i];     
      acc1 = acc1 + data[i+stride];
      acc2 = acc2 + data[i+sx2]; 
      acc3 = acc3 + data[i+sx3];
    }

    /* Finish any remaining elements */
    for (; i < length; i++) {
	acc0 = acc0 + data[i];
    }
    return ((acc0 + acc1) + (acc2 + acc3));
}

/*
int test(int *data, int elems, int stride)

{
	// PUT HERE YOUR CODE
	// THAT RETURNS THE SUM
	// OF THE ARRAY data[0:elems] with stride "stride"

  int sum = 0;
  for ( int ii = 0; ii < elems; ii += stride )
    sum += data[ii];
  
  return sum;
}
*/


