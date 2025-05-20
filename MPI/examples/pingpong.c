
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"

#define COUNTS 100


int main(int argc,char **argv)

{
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
  if ( provided < MPI_THREAD_FUNNELED )
    {
      // manage
      MPI_Abort(MPI_COMM_WORLD, 1);
    }

  max_size = ( argc > 1 ? atoi(*(argv+1)) : 20 );
  
  int Myrank, Ntasks;
  MPI_Comm myCOMM_WORLD;

  MPI_Comm_dup ( MPI_COMM_WORLD, &myCOMM_WORLD );
  MPI_Comm_size( myCOMM_WORLD, &Ntasks );    
  MPI_Comm_rank( myCOMM_WORLD, &Myrank );     

  
/**** your code here ****/
  double timing[max_size] = {0};
  max_size = ( 1 << max_size);
  char *buffer_send = (char*)malloc( max_size );
  char *buffer_recv = (char*)malloc( max_size );
  
  if ( Rank == 0 )
    {
      for ( int j = 1; j < max_size; j++ )
	buffer_send[j] = 1;  // not so important, though
      
      for ( int j = 1; j < max_size; j<<1 )
	{
	  timing[j] = MPI_Wtime();

	  for ( int n = 0; n < COUNTS; n++ )
	    {
	      MPI_Send(   // ... complete
                      );

	      MPI_Recv(   // ... complete
                      );
	    }
	  timing[j] = (MPI_Wtime()-timing[j]) / COUNTS;
	}

      // print results
      
    }
  else if ( Rank == 1)
    {
      // complete
    }
  }

  MPI_Finalize();
  return 0;
}
