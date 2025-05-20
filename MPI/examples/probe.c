

#include <stdlib.h>                                                                                                   
#include <stdio.h>
#include <time.h>
#include <mpi.h>                                                                                                      

#define TAG_DATA 1
                                                                                                                      
int main ( int argc, char **argv )                                                                                    
{
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);
  if ( provided < MPI_THREAD_SINGLE )
    {
      // manage
      MPI_Abort();
    }
                                                                                                                      
  int Me, Ntasks;                                                                                                     
                                                                                                                      
  MPI_Comm_size( MPI_COMM_WORLD, &Ntasks );                                                                           
  MPI_Comm_rank( MPI_COMM_WORLD, &Me );                                                                               
                                                                                                                      
  char myname[10], myname2[10];                                                                                       
                                                                                                                      
  if ( Me == 0 )                                                                                                      
    {                                                                                                                 
      srand(time(NULL));
      int N     = rand() % 769;
      int *data = (int*)malloc( sizeof(int) * N);
      for ( int i = 0; i < N; i++ )
	data[i] = rand();

      MPI_Send( data, N, MPI_INT, 1, TAG_DATA, MPI_COMM_WORLD );
      printf("Task %d has sent %d data: %d %d, ... %d!ì\n",
	     Me, N, data[0], data[1], data[N-1] );

      free( data );
    }                                                                                                                 
  else                                                                                                                
    {
      MPI_Status status;
      int Sender;
      int Tag;
      int N;
      
      MPI_Probe( MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
      MPI_Get_count ( &status, MPI_INT, &N );

      int *local = (int*)malloc( sizeof(int) * N );
      MPI_Recv( local, N, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status );

      // is that correct to access the data array before knowing exactly
      // how many data have been actually received ?
      //
      printf("Task %d has received %d data: %d %d, ... %d\n",
	     Me, N, local[0], local[1], local[N-1] );

      free( local );      
      
    }

  MPI_Finalize();
  return 0;
}
