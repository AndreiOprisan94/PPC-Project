#include <fstream>
#include "utils.h"

using namespace std;

int** allocateMatrix(int matrixDimension) {
    int** matrix = new int*[matrixDimension];

    for (int i = 0; i < matrixDimension; i++) {
        matrix[i] = new int[matrixDimension];
    }

    return matrix;
}

void freeMatrix(int** matrix, int matrixDimension) {
    for(int i = 0; i < matrixDimension; i++) {
        delete[] matrix[i];
    }

    delete[] matrix;
}

void populateMatrixWithOnes(int** matrix, int matrixDimension) {
    for (int i = 0; i < matrixDimension; i++) {
        for (int j = 0; j < matrixDimension; ++j) {
            matrix[i][j] = 1;
        }
    }
}

void printMatrix(ofstream& destination, int** matrix, int matrixDimension) {
    for (int i = 0; i < matrixDimension; ++i) {
        for (int j = 0; j < matrixDimension; ++j) {
            destination << matrix[i][j] << " ";
        }
        destination << endl;
    }
}