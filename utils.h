#ifndef PARALLELMATRIXMULTIPLICATION_UTILS_H
#define PARALLELMATRIXMULTIPLICATION_UTILS_H

#include <fstream>

int** allocateMatrix(int matrixDimension);
void freeMatrix(int** matrix, int matrixDimension);
void printMatrix(std::ofstream& destination, int** matrix, int matrixDimension);
void populateMatrixWithOnes(int** matrix, int matrixDimension);

#endif
