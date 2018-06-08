#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <utility>

#include "mpi.h"
#include "utils.h"

#define MASTER 0
#define ROW 1
#define COLUMN 2

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
    cout << "I am the master! And the matrixDimension is " << matrixDimension << endl << flush;

    int **firstMatrix = allocateMatrix(matrixDimension);
    int **secondMatrix = allocateMatrix(matrixDimension);
    int **matrixMultiplicationResult = allocateMatrix(matrixDimension);

    populateMatrixWithOnes(firstMatrix, matrixDimension);
    populateMatrixWithOnes(secondMatrix, matrixDimension);

    cout << "I, the master, am sending the data to the slaves" << endl << flush;

    int slave = 1;

    for (int i = 0; i < matrixDimension; i++) {
        for (int j = 0; j < matrixDimension; ++j) {
            if (slave < worldSize) {
                MPI_Send(firstMatrix[i], matrixDimension, MPI_INT, slave, ROW, MPI_COMM_WORLD);
                MPI_Send(&secondMatrix[j][0], matrixDimension, MPI_INT, slave, COLUMN, MPI_COMM_WORLD);
                slave++;
            } else {
                slave = 1;
            }
        }
    }
    
    freeMatrix(firstMatrix, matrixDimension);
    freeMatrix(secondMatrix, matrixDimension);
    freeMatrix(matrixMultiplicationResult, matrixDimension);

}

static void slaveProcess(int matrixDimension, int worldRank) {
    int *row = new int[matrixDimension];
    int *column = new int[matrixDimension];

    while (true) {
        MPI_Status status;
        MPI_Recv(row, matrixDimension, MPI_INT, MASTER, ROW, MPI_COMM_WORLD, &status);
        MPI_Recv(column, matrixDimension, MPI_INT, MASTER, COLUMN, MPI_COMM_WORLD, &status);

        cout << "I am slave number: " << worldRank << endl << flush;

        cout << "Row content is: ";
        printLine(cout, row, matrixDimension);

        cout << "Column content is: ";
        printLine(cout, row, matrixDimension);
    }

    delete[] row;
    delete[] column;
}