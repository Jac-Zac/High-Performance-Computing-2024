
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <mpi.h>


int main ( int argc, char **argv )
{

  int Rank;
  int Ntasks;

  MPI_Init ( &argc, &argv );

  MPI_Comm_rank( MPI_COMM_WORLD, &Rank );
  MPI_Comm_size( MPI_COMM_WORLD, &Ntasks );

  char hostname[100];
  int hostname_size = 100;
  gethostname( &hostname[0], hostname_size );

  printf( "Rank %d is running on host %s\n", Rank, hostname );

  MPI_Barrier(MPI_COMM_WORLD);
  
  int Im_even = (Rank % 2 == 0);
  int my_buddy;
  if ( Im_even )
    my_buddy = Rank+1;
  else
    my_buddy = Rank-1;

  if ( my_buddy < Ntasks )
    {

      #define FIRST_ROUND 0
      #define SECOND_ROUND 1
      
      if ( Im_even )
	{
	  int        buddy_rank;
	  MPI_Status status;

	  MPI_Send( &Rank, 1, MPI_INT, my_buddy, FIRST_ROUND, MPI_COMM_WORLD );
	  MPI_Recv( &buddy_rank, 1, MPI_INT, my_buddy, SECOND_ROUND, MPI_COMM_WORLD, &status );
	  

	  if ( buddy_rank == my_buddy )
	    printf("\tRank %d says: I confirm I have received a reply from my buddy\n", Rank);
	  else
	    printf("oops, a stranger has replied to me\n");	  
	}
      else
	{
	  int        buddy_rank;
	  MPI_Status status;

	  MPI_Recv( &buddy_rank, 1, MPI_INT, my_buddy, FIRST_ROUND, MPI_COMM_WORLD, &status );
	  MPI_Send( &Rank, 1, MPI_INT, my_buddy, SECOND_ROUND, MPI_COMM_WORLD );
	  

	  if ( buddy_rank == my_buddy )
	    printf("Rank %d says: I confirm I have received the message from my buddy\n", Rank);
	  else
	    printf("oops, a stranger has communicated with me\n");	  
	}
      
    }
  else
    printf("Rank %d has got nobody to talk with\n", Rank );
    

  MPI_Finalize();

  return 0;
}
