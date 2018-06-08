#include <iostream>
#include <map>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "mpi.h"
#include "utils.h"

#define MASTER 0
#define ROW 1
#define COLUMN 2
#define END 3

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

    char dummyChar = ' ';

    //Send finish message to slaves, two messages because it expects a row and a column
    for (int k = 1; k < slave; ++k) {
        MPI_Send(&dummyChar, 1, MPI_BYTE, k, END, MPI_COMM_WORLD);
        MPI_Send(&dummyChar, 1, MPI_BYTE, k, END, MPI_COMM_WORLD);
    }

    for (int k = slave; k < worldSize; ++k) {
        MPI_Send(&dummyChar, 1, MPI_BYTE, k, END, MPI_COMM_WORLD);
        MPI_Send(&dummyChar, 1, MPI_BYTE, k, END, MPI_COMM_WORLD);
    }
    
    freeMatrix(firstMatrix, matrixDimension);
    freeMatrix(secondMatrix, matrixDimension);
    freeMatrix(matrixMultiplicationResult, matrixDimension);

}

static void slaveProcess(int matrixDimension, int worldRank) {
    int *row = new int[matrixDimension];
    int *column = new int[matrixDimension];
    
    bool workToDo = true;

    while (workToDo) {
        MPI_Status rowStatus;
        MPI_Status columnStatus;

        MPI_Recv(row, matrixDimension, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &rowStatus);
        MPI_Recv(column, matrixDimension, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &columnStatus);

        cout << "I am slave number: " << worldRank << endl << flush;

        if (rowStatus.MPI_TAG != END && columnStatus.MPI_TAG != END) {
            if (rowStatus.MPI_TAG == ROW) {
                cout << "Row content is: ";
                printLine(cout, row, matrixDimension);
            }

            if (columnStatus.MPI_TAG == COLUMN) {
                cout << "Column content is: ";
                printLine(cout, column, matrixDimension);
            }
        } else {
            workToDo = false;
        }
    }

    cout << "I am slave number: " << worldRank << " and I finished processing"<<endl << flush;

    delete[] row;
    delete[] column;
}