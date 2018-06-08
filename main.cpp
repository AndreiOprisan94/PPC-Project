#include <iostream>
#include <unordered_map>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "mpi.h"
#include "utils.h"

#define MASTER 0
#define ROW 1
#define COLUMN 2
#define RESULT 3
#define END 4

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

    int slaveCount = 1;
    unordered_map<int, pair<int,int> > slavesInformation;

    for (int i = 0; i < matrixDimension; i++) {
        for (int j = 0; j < matrixDimension; ++j) {
            if (slaveCount < worldSize) {
                pair<int, int> position;
                position.first = i;
                position.second = j;
                slavesInformation[slaveCount] = position;

                MPI_Send(firstMatrix[i], matrixDimension, MPI_INT, slaveCount, ROW, MPI_COMM_WORLD);
                MPI_Send(&secondMatrix[j][0], matrixDimension, MPI_INT, slaveCount, COLUMN, MPI_COMM_WORLD);
                slaveCount++;
            } else {
                for (int slave = 1; slave < worldSize; ++slave) {
                    int result;
                    MPI_Status resultMessageStatus;
                    MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE, RESULT, MPI_COMM_WORLD, &resultMessageStatus);

                    pair<int, int> position = slavesInformation[resultMessageStatus.MPI_SOURCE];
                    matrixMultiplicationResult[position.first][position.second] = result;
                }

                slaveCount = 1;
            }
        }
    }

    char dummyChar = ' ';

    //Send finish message to slaves, two messages because it expects a row and a column
    for (int k = 1; k < slaveCount; ++k) {
        MPI_Send(&dummyChar, 1, MPI_BYTE, k, END, MPI_COMM_WORLD);
        MPI_Send(&dummyChar, 1, MPI_BYTE, k, END, MPI_COMM_WORLD);
    }

    for (int k = slaveCount; k < worldSize; ++k) {
        MPI_Send(&dummyChar, 1, MPI_BYTE, k, END, MPI_COMM_WORLD);
        MPI_Send(&dummyChar, 1, MPI_BYTE, k, END, MPI_COMM_WORLD);
    }

    cout << "Matrix multiplication result is: " << endl;
    printMatrix(cout, matrixMultiplicationResult, matrixDimension);
    
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
            int resultForMaster = multiplyVectors(row, column, matrixDimension);
            MPI_Send(&resultForMaster, 1, MPI_INT, MASTER, RESULT, MPI_COMM_WORLD);
        } else {
            workToDo = false;
        }
    }

    cout << "I am slave number: " << worldRank << " and I finished processing"<<endl << flush;

    delete[] row;
    delete[] column;
}