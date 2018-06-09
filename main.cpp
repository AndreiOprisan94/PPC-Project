#include <iostream>
#include <unordered_map>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "mpi.h"
#include "utils.h"

#define MASTER 0
#define RESULT 1

using namespace std;

static void masterProcess(int matrixDimension, int worldSize);
static void slaveProcess(int matrixDimension);

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
        const clock_t begin = clock();
        masterProcess(matrixDimension, worldSize);
        const clock_t end = clock();
        const double timeSpent = ((double) end - begin) / CLOCKS_PER_SEC;

        printf("[PARALLEL] Time=%f&MatrixSize=%d", timeSpent, matrixDimension);
        fflush(stdout);
    } else {
        slaveProcess(matrixDimension);
    }

    MPI_Finalize();

    return 0;
}

static void masterProcess(int matrixDimension, int worldSize) {
    cout << "[Parallel] Matrix multiplication with square matrix of dimension: " << matrixDimension << endl << flush;

    int **firstMatrix = allocateMatrix(matrixDimension);
    int **secondMatrix = allocateMatrix(matrixDimension);
    int **matrixMultiplicationResult = allocateMatrix(matrixDimension);

    populateMatrixWithOnes(firstMatrix, matrixDimension);
    populateMatrixWithOnes(secondMatrix, matrixDimension);

    for (int i = 0; i < matrixDimension; ++i) {
        MPI_Bcast(firstMatrix[i], matrixDimension, MPI_INT, MASTER, MPI_COMM_WORLD);
        MPI_Bcast(secondMatrix[i], matrixDimension, MPI_INT, MASTER, MPI_COMM_WORLD);
    }

    int numberOfLines = matrixDimension / worldSize;
    cout << "Number of lines: " << numberOfLines << endl << flush;
    for (int i = 0; i < numberOfLines; ++i) {
        for (int j = 0; j < matrixDimension; ++j) {
            int result = 0;
            for (int counter = 0; counter < matrixDimension; ++counter) {
                result += firstMatrix[i][counter] * secondMatrix[counter][j];
            }
            matrixMultiplicationResult[i][j] = result;
        }
    }

    int i = numberOfLines;
    for (int slave = 1; slave < worldSize; ++slave) {
        for (int l = 0; l < numberOfLines; ++l) {
            MPI_Status status;
            MPI_Recv(matrixMultiplicationResult[i], matrixDimension, MPI_INT, slave, RESULT, MPI_COMM_WORLD, &status);
            i++;
        }
    }

    cout << "The matrix is: " << endl << flush;
    printMatrix(cout, matrixMultiplicationResult, matrixDimension);

    freeMatrix(firstMatrix, matrixDimension);
    freeMatrix(secondMatrix, matrixDimension);
    freeMatrix(matrixMultiplicationResult, matrixDimension);

}

static void slaveProcess(int matrixDimension) {
    int **firstMatrix = allocateMatrix(matrixDimension);
    int **secondMatrix = allocateMatrix(matrixDimension);

    for (int i = 0; i < matrixDimension; ++i) {
        MPI_Bcast(firstMatrix[i], matrixDimension, MPI_INT, MASTER, MPI_COMM_WORLD);
        MPI_Bcast(secondMatrix[i], matrixDimension, MPI_INT, MASTER, MPI_COMM_WORLD);
    }

    int worldSize;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int numberOfLines = matrixDimension / worldSize;
    int **resultStorage = new int*[numberOfLines];

    for (int i = 0; i < numberOfLines; ++i) {
        resultStorage[i] = new int[matrixDimension];
    }

    int offset = numberOfLines * rank;
    for (int line = offset; line < offset + numberOfLines; line++) {
        for (int column = 0; column < matrixDimension; ++column) {
            int result = 0;
            for (int counter = 0; counter < matrixDimension; ++counter) {
                result += firstMatrix[line][counter] * secondMatrix[counter][column];
            }
            resultStorage[line-offset][column] = result;
        }
    }

    for (int i = 0; i < numberOfLines; ++i) {
        MPI_Send(resultStorage[i], matrixDimension, MPI_INT, MASTER, RESULT, MPI_COMM_WORLD);
    }

    freeMatrix(firstMatrix, matrixDimension);
    freeMatrix(secondMatrix, matrixDimension);

    for (int i = 0; i < numberOfLines; ++i) {
        delete[] resultStorage[i];
    }

    delete[] resultStorage;
}