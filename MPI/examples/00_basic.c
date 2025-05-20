
#include <stdlib.h>
#include <stdio.h>

#include <mpi.h>


int main ( int argc, char **argv )
{

  printf ("--> argc is %d\n", argc );
  int Rank;
  int Ntasks;

  MPI_Init ( &argc, &argv );

  MPI_Comm_rank( MPI_COMM_WORLD, &Rank );
  MPI_Comm_size( MPI_COMM_WORLD, &Ntasks );

  int planets;
  int myplanets;
  
  planets = (argc > 1? atoi(*(argv+1)) : 0 );

  myplanets = planets / Ntasks;
  
  printf( "Hello world, I'm task %d among %d friends, i'll model %d planets\n",
	  Rank, Ntasks, myplanets );
  

  MPI_Finalize();

  return 0;
}
