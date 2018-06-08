#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <utility>

#include "mpi.h"
#include "utils.h"

#define MASTER 0

using namespace std;

static void masterProcess(int matrixDimension, int worldSize);
static void slaveProcess(int matrixDimension, int worldRank);

int main(int argc, char* argv[]) {

    if (argc < 2) {
        cerr << "Please provide a dimension for the matrix" << endl << flush;
        return -1;
    }

    MPI_Init(&argc, &argv);

    int matrixDimension = atoi(argv[1]);

    int worldRank, worldSize;

    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    if (worldRank == MASTER) {
        masterProcess(matrixDimension, worldSize);
    } else {
        slaveProcess(matrixDimension, worldRank);
    }

    MPI_Finalize();

    return 0;
}

static void masterProcess(int matrixDimension, int worldSize) {
    cout << "I am the master!" << endl << flush;

    int **firstMatrix = allocateMatrix(matrixDimension);
    int **secondMatrix = allocateMatrix(matrixDimension);
    int **matrixMultiplicationResult = allocateMatrix(matrixDimension);

    populateMatrixWithOnes(firstMatrix, matrixDimension);
    populateMatrixWithOnes(secondMatrix, matrixDimension);

    int *rows = new int[matrixDimension];
    int *columns = new int[matrixDimension];
    int counter = 0;

    while (counter < matrixDimension) {
        for (int i = 1; i < worldSize && counter < matrixDimension; i++) {
            for (int j = 0; j < matrixDimension; j++) {
                rows[j] = firstMatrix[counter][j];
                columns[j] = secondMatrix[j][counter];
            }

            MPI_Send(rows, matrixDimension, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(columns, matrixDimension, MPI_INT, i, 0, MPI_COMM_WORLD);
            

        }
    }

    cout << "I, the master, successfully send the numbers" << endl << flush;


    freeMatrix(firstMatrix, matrixDimension);
    freeMatrix(secondMatrix, matrixDimension);
    freeMatrix(matrixMultiplicationResult, matrixDimension);

}

static void slaveProcess(int matrixDimension, int worldRank) {
    cout << "I am slave number: " << worldRank << endl << flush;
    int integerToReceive;
    MPI_Status status;
    MPI_Recv(&integerToReceive, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

    cout << "I, " << worldRank << " received the number " << integerToReceive << " from: " << status.MPI_SOURCE << endl << flush;
}