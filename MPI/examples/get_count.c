

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAG_DATA 1

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int Me, Ntasks;

  MPI_Comm_size(MPI_COMM_WORLD, &Ntasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &Me);

  int Ndata = (argc > 1 ? atoi(*argv + 1) : 100);
  char *data = (char *)malloc(Ndata);

  for (int i = 0; i < Ndata; i++)
    data[i] = Me;

  if (Me == 0) {
    MPI_Send(data, Ndata, MPI_BYTE, 1, TAG_DATA, MPI_COMM_WORLD);
    printf("Task %d has sent %d data: %d %d, ... %d!ì\n", Me, Ndata, data[0],
           data[1], data[Ndata - 1]);
  } else {
    MPI_Status status;
    int Nrecv;

    MPI_Recv(data, Ndata, MPI_BYTE, 0, TAG_DATA, MPI_COMM_WORLD, &status);

    MPI_Get_count(&status, MPI_BYTE, &Nrecv);
    printf("Task %d has received %d data: %d %d, ... %d\n", Me, Nrecv, data[0],
           data[1], data[Nrecv - 1]);
  }

  free(data);

  MPI_Finalize();
  return 0;
}
