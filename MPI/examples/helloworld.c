#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main ( int argc, char **argv )
{
   int provided_thread_level;
   MPI_Comm myCOMM_WORLD;
   MPI_Init_thread( &argc, &argv, MPI_THREAD_SINGLE, &provided_thread_level );
   
   if ( provided_thread_level < MPI_THREAD_SINGLE )
     {
       printf("it was impossibile to get the thread level MPI_THREAD_SINGLE\n");
       MPI_Abort(MPI_COMM_WORLD, 1);
     }


   int Ntasks;
   int Myrank;
   MPI_Comm_dup( MPI_COMM_WORLD, &myCOMM_WORLD );
   MPI_Comm_size( myCOMM_WORLD, &Ntasks );
   MPI_Comm_rank( myCOMM_WORLD, &Myrank );
   
   printf("hello MPI world, I'm rank %d out of %d\n", Myrank, Ntasks );
   MPI_Finalize();
   
   return 0;
}

