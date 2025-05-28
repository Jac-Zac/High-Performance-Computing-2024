
/* ·························································································
 *
 *  This code provides and example on the usage of MPI_Alltoall and on how to achieve the
 *  same result with
 *  1)  a combination of Isend / Irecv
 *  2)  a combination of Isend / Recv( MPI_ANY_SOURCE )
 *
 *  you can select which one to use providing a command line argument:
 *  0  --> run with MPI_Alltoall
 *  1  --> run with Isend / Irecv
 *  2  --> run with Isend / Recv( ANY )
 *
 *  This code is provided in the frame of the course
 *   " Foundations of HPC 2024 / 2025"
 *  at University of Trieste
 * 
 *  contact: luca.tornatore@inaf.it
 *  
 *
 * ·························································································· */



#if defined(__STDC__)
#  if (__STDC_VERSION__ >= 199901L)
#     define _XOPEN_SOURCE 700
#  endif
#endif
#include <stdlib.h>     
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <mpi.h>


#define RUN_WITH_ALLTOALL       0
#define RUN_WITH_ISEND_IRECV    1
#define RUN_WITH_ISEND_RECVANY  2


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

  int      mode = (argc > 1 ? atoi(*(argv+1)) : 0 );
  // 
  int      Myrank, Ntasks;
  MPI_Comm myCOMM_WORLD;

  MPI_Comm_dup ( MPI_COMM_WORLD, &myCOMM_WORLD );
  MPI_Comm_size( myCOMM_WORLD, &Ntasks );    
  MPI_Comm_rank( myCOMM_WORLD, &Myrank );     

  /* ·····························································
   *  prepare the data
   */
  
  data_t *mydata  = (data_t*)malloc( 2 * sizeof(data_t) * Ntasks );   // we allocate 2 buffers: the first half
  data_t *alldata = mydata + Ntasks;                                  // contains the data to be sent, the second half
                                                                      // the data to be received
                                                                      //
  
  for ( int i = 0; i < Ntasks; i++ )
    {
      mydata[i].data[0] = Myrank;      // the first int is the sender's rank
      mydata[i].data[1] = i;           // the second int is the target's rank
    }



  /* ·····························································
   *  exchange the data
   */

  if (Myrank == 0 )
    // NOTE: strictly speaking, we do not know whether ALL the tasks are at this point;
    // we sould need to post an MPI_Barrier, ar a collective of some kind to ensure that.
    // However, for practical purposes, letting one task to issue a progression message is
    // enough.
    // For debugging purposes, you may let all the ranks to print a message.
    //
    printf ( "data exchange starts\n");
  
  
  switch ( mode )
    {

      // ----------------       ALL_TO_ALL
    case RUN_WITH_ALLTOALL: {
      MPI_Alltoall ( mydata, 2, MPI_INT, alldata, 2, MPI_INT, myCOMM_WORLD );
      
    } break;

      // ----------------       ISEND_IRECV
    case RUN_WITH_ISEND_IRECV: {

      // Isending
      //
      for ( int i = 0, r = 0; i < Ntasks; i++ )
	if ( i != Myrank )  // we skip sending data to ourselves
	  {
	    MPI_Request srequest;
	    MPI_Isend( &mydata[i], 2, MPI_INT, i, 0, myCOMM_WORLD, &srequest );
	    // we do not actually need to know anything about the sending;
	    // as such, we can free the request immediately and re-use it
	    //
	    MPI_Request_free( &srequest );
	  }

      // Ireceiving
      //
      MPI_Request *rrequests;
      MPI_Status  *rstatus;
      rrequests = (MPI_Request*)malloc( sizeof(MPI_Request) * (Ntasks-1) );
      rstatus   = (MPI_Status*)malloc( sizeof(MPI_Status) * (Ntasks-1) );

      for ( int i = 0, r = 0; i < Ntasks; i++ )
	if ( i != Myrank ) // we skip receiving data from ourselves
	  MPI_Irecv( &alldata[i], 2, MPI_INT, i, 0, myCOMM_WORLD,  &rrequests[r++] );

      // waiting to receive the data
      //
      MPI_Waitall( Ntasks-1, rrequests, rstatus );

      // receive requests have been tested and conmpleted
      // we can nt use MPI_Request_free, we just free the memory
      free ( rrequests );	  
      free ( rstatus );

      // since we have not sent/received data to/from ourseleves, we need
      // to manually propagate our data into alldata
      memcpy ( &alldata[ Myrank ], &mydata [ Myrank ], sizeof( data_t ) );

    } break;

      // ----------------       ISEND_RECVANY
    case RUN_WITH_ISEND_RECVANY: {

      // Isending
      //
      for ( int i = 0, r = 0; i < Ntasks; i++ )
	if ( i != Myrank )
	  {
	    MPI_Request srequest;
	    MPI_Isend( &mydata[i], 2, MPI_INT, i, 0, myCOMM_WORLD, &srequest );
	    // we do not actually need to know anything about the sending;
	    // as such, we can free the request immediately and re-use it
	    //
	    MPI_Request_free( &srequest );	    
	  }

      // receiving
      //
      for ( int i = 0, r = 0; i < Ntasks-1; i++ )
	{
	  int        buffer[2];
	  MPI_Status status;
	  MPI_Recv( buffer, 2, MPI_INT, MPI_ANY_SOURCE, 0, myCOMM_WORLD, &status );

	  alldata[ status.MPI_SOURCE ].data[0] = buffer[0];
	  alldata[ status.MPI_SOURCE ].data[1] = buffer[1];
	}

      // since we have not sent/received data to/from ourseleves, we need
      // to manually propagate our data into alldata
      memcpy ( &alldata[ Myrank ], &mydata [ Myrank ], sizeof( data_t ) );

    } break;

    }

  if (Myrank == 0 )
    // NOTE: strictly speaking, we do not know whether ALL the tasks are at this point;
    // we sould need to post an MPI_Barrier, ar a collective of some kind to ensure that.
    // However, for practical purposes, letting one task to issue a progression message is
    // enough.
    // For debugging purposes, you may let all the ranks to print a message.
    //
    printf ( "data exchange done\n");

  /* ·····························································
   *  verify that everything went on smoothly
   */

  if (Myrank == 0 )
    printf ( "verifying data.. \n");
  
  fails_t fails = {0,0};
  for ( int i = 0; i < Ntasks; i++ )
    {
      fails.sender_fails += alldata[i].data[0] != i;
      fails.receiver_fails += alldata[i].data[1] != Myrank;
    }

  // everybody want to know the truth, so we propgate via an Allreduce
  //
  MPI_Allreduce( MPI_IN_PLACE, &fails, 2, MPI_INT, MPI_SUM, myCOMM_WORLD );

  if ( Myrank == 0) {
    if ( (fails.sender_fails > 0) ||
	 (fails.receiver_fails > 0) )
      printf("oops... %u and %u sender / receiver failures\n", fails.sender_fails, fails.receiver_fails );
    else
      printf("everything was good\n"); }

  /* ·····························································
   *  clean up
   */

  free ( mydata );
    

  /* ·····························································
   *  that's all folks
   */
      
  MPI_Finalize();
  return 0;
}
