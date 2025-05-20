#if defined(__STDC__)
#  if (__STDC_VERSION__ >= 199901L)
#     define _XOPEN_SOURCE 700
#  endif
#endif
#define _GNU_SOURCE
#include <stdlib.h>     
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#if defined(_GNU_SOURCE)
#include <sched.h>
#endif
#include <omp.h>
#include <mpi.h>




#define HOST_NAME_MAX      200

int read_proc__self_stat ( int, int * );
int get_cpu_id           ( void       );


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

  char myhostname[HOST_NAME_MAX+1];
  gethostname( myhostname, HOST_NAME_MAX );


  #pragma omp parallel
  {
    int me    = omp_get_thread_num();
    int mycpu = get_cpu_id();
    
    printf("T %03d th %03d cpu %03d on host %s\n", Myrank, me, mycpu, myhostname);
  }
  
  MPI_Finalize();
  return 0;
}



#define CPU_ID_ENTRY_IN_PROCSTAT 39


int get_cpu_id( void )
{
 #if defined(_GNU_SOURCE)                              // GNU SOURCE ------------
  
  return  sched_getcpu( );

 #else

 #ifdef SYS_getcpu                                     //     direct sys call ---
  
  int cpuid;
  if ( syscall( SYS_getcpu, &cpuid, NULL, NULL ) == -1 )
    return -1;
  else
    return cpuid;
  
 #else      

  unsigned val;
  if ( read_proc__self_stat( CPU_ID_ENTRY_IN_PROCSTAT, &val ) == -1 )
    return -1;

  return (int)val;

 #endif                                                // -----------------------
 #endif

}



int read_proc__self_stat( int field, int *ret_val )
/*
  Other interesting fields:

  pid      : 0
  father   : 1
  utime    : 13
  cutime   : 14
  nthreads : 18
  rss      : 22
  cpuid    : 39

  read man /proc page for fully detailed infos
 */
{
  // not used, just mnemonic
  // char *table[ 52 ] = { [0]="pid", [1]="father", [13]="utime", [14]="cutime", [18]="nthreads", [22]="rss", [38]="cpuid"};

  *ret_val = 0;

  FILE *file = fopen( "/proc/self/stat", "r" );
  if (file == NULL )
    return -1;

  char   *line = NULL;
  int     ret;
  size_t  len;
  ret = getline( &line, &len, file );
  fclose(file);

  if( ret == -1 )
    return -1;

  char *savetoken = line;
  char *token = strtok_r( line, " ", &savetoken);
  --field;
  do { token = strtok_r( NULL, " ", &savetoken); field--; } while( field );

  *ret_val = atoi(token);

  free(line);

  return 0;
}

