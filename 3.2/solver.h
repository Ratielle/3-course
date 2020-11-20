#include "matrix.h"
#include <vector>

namespace Solver {
void AlignRow(Matrix &row, const std::vector<Matrix *> &rest);
void Align(Matrix &A, Matrix *B = nullptr);
void QuasiTriangulate(Matrix &A, Matrix *B = nullptr);
Matrix DecomposeLR(Matrix &A);
std::vector<double> EigenValues(Matrix &A);
}; // namespace Solver