#if defined(__STDC__)
#  if (__STDC_VERSION__ >= 199901L)
#     define _XOPEN_SOURCE 700
#  endif
#endif
#include <stdlib.h>     
#include <stdio.h>
#include <omp.h>
#include <mpi.h>


typedef struct { int data[2]; } data_t;
typedef struct {
    unsigned int sender_fails;
  unsigned int receiver_fails; } fails_t;

  
int main( int argc, char **argv )
{

  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
  if ( provided < MPI_THREAD_FUNNELED )
    {
      // manage
      MPI_Abort(MPI_COMM_WORLD, 1);
    }

  int Myrank, Ntasks;
  MPI_Comm myCOMM_WORLD;

  MPI_Comm_dup ( MPI_COMM_WORLD, &myCOMM_WORLD );
  MPI_Comm_size( myCOMM_WORLD, &Ntasks );    
  MPI_Comm_rank( myCOMM_WORLD, &Myrank );     

  data_t *mydata = (data_t*)malloc( 2 * sizeof(data_t) * Ntasks );
  data_t *alldata = mydata + Ntasks;

  for ( int i = 0; i < Ntasks; i++ )
    {
      mydata[i].data[0] = Myrank;
      mydata[i].data[1] = i;      
    }




  // checking

  fails_t fails = {0,0};
  for ( int i = 0; i < Ntasks; i++ )
    {
      fails.sender_fails += alldata[i].data[0] != i;
      fails.receiver_fails += alldata[i].data[1] != Myrank;
    }

  MPI_Allreduce( MPI_IN_PLACE, &fails, 2, MPI_INT, MPI_SUM, myCOMM_WORLD );

  if ( (fails.sender_fails > 0) ||
       (fails.receiver_fails > 0) )
    if ( Myrank == 0 )
      printf("oops... %u and %u sender / receiver failures\n", fails.sender_fails, fails.receiver_fails );
  
  free ( mydata );

  MPI_Finalize();
  return 0;
}
