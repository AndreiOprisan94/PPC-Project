#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "mpi.h"

using namespace std;

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int worldRank, worldSize;

    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    if (worldRank == 0) {
        cout << "I am the master!" << endl << flush;

        for (int i = 1; i < worldSize; i++) {
            int integerToSend = i+2;
            MPI_Send(&integerToSend, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        cout << "I, the master, successfully send the numbers" << endl << flush;
    } else {
        cout << "I am not the master I am " << worldRank << endl << flush;
        int integerToReceive;
        MPI_Status status;
        MPI_Recv(&integerToReceive, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

        cout << "I, " << worldRank << " received the number " << integerToReceive << " from: " << status.MPI_SOURCE << endl << flush;
    }

    MPI_Finalize();

    return 0;
}