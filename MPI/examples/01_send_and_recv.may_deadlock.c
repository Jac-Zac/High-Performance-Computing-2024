
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define HNAME_LEN 100

void get_hostname(char *name) {
  int hostname_len = HNAME_LEN;
  gethostname(name, hostname_len);
  return;
}

int main(int argc, char **argv) {

  char hostname[HNAME_LEN];
  get_hostname(hostname);

  int Rank;
  int Ntasks;
  int MsgSize;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
  MPI_Comm_size(MPI_COMM_WORLD, &Ntasks);

  MPI_Barrier(MPI_COMM_WORLD);

  int Im_even = (Rank % 2 == 0);
  int my_buddy;
  if (Im_even)
    my_buddy = Rank + 1;
  else
    my_buddy = Rank - 1;

  if (my_buddy < Ntasks) {

#define FIRST_ROUND 0
#define SECOND_ROUND 1

    /*
     * here it is highly possible (not guaranteed) that we end in a deadlock,
     * because the MPI_Send are posted concurrently.
     * The MPI specifications leave to the implementer to decide whether
     * MPI_Send is _blocking_ in the sense that it will never return before the
     * reception signal has been posted OR before the availability of the
     * local sent buffer.
     * The point is that the beheviour may depends on the SIZE of the message.
     * For short-enough messages (i.e. few KB), the data are sent immediately
     * and some caching happens at receiver-size if the corresponding Recv
     * (i.e. the dedicated memory space has not been guaranteed) has not been
     * posted yet.
     * That is called "eager" protocal.
     * For larger messages, a "rendezvous" protocal may be adopted: only the
     * requests for sending are posted, and the actual send happens when the
     * corresponding Recv is issued.
     * NOTE: what is "small" and  "large" is implementation dependent.
     *
     */

    if (Im_even) {
      int buddy_rank;
      MPI_Status status;

      MPI_Send(&Rank, 1, MPI_INT, my_buddy, FIRST_ROUND, MPI_COMM_WORLD);
      MPI_Recv(&buddy_rank, 1, MPI_INT, my_buddy, SECOND_ROUND, MPI_COMM_WORLD,
               &status);

      if (buddy_rank == my_buddy)
        printf(
            "\tRank %d says: I confirm I have received a reply from my buddy\n",
            Rank);
      else
        printf("oops, a stranger has replied to me\n");
    } else {
      int buddy_rank;
      MPI_Status status;

      MPI_Send(&Rank, 1, MPI_INT, my_buddy, SECOND_ROUND, MPI_COMM_WORLD);
      MPI_Recv(&buddy_rank, 1, MPI_INT, my_buddy, FIRST_ROUND, MPI_COMM_WORLD,
               &status);

      if (buddy_rank == my_buddy)
        printf("Rank %d says: I confirm I have received the message from my "
               "buddy\n",
               Rank);
      else
        printf("oops, a stranger has communicated with me\n");
    }

  } else
    printf("Rank %d has got nobody to talk with\n", Rank);

  MPI_Finalize();

  return 0;
}
