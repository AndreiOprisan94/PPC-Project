#ifndef PARALLELMATRIXMULTIPLICATION_UTILS_H
#define PARALLELMATRIXMULTIPLICATION_UTILS_H

#include <fstream>

int** allocateMatrix(int matrixDimension);
void freeMatrix(int** matrix, int matrixDimension);
void printMatrix(std::ostream& destination, int** matrix, int matrixDimension);
void printLine(std::ostream& destination, int* line, int lineSize);
void populateMatrixWithOnes(int** matrix, int matrixDimension);

#endif
