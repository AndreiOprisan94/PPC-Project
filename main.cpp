#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "mpi.h"

using namespace std;

int main(int argc, char* argv[]) {

    cout << "It works!" << endl << flush;

    int rank, size, len;
    char version[MPI_MAX_LIBRARY_VERSION_STRING];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_library_version(version, &len);
    printf("Hello, world, I am %d of %d, (%s, %d)\n",
           rank, size, version, len);
    MPI_Finalize();

    return 0;
}