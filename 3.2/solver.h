#include "matrix.h"
#include <vector>

namespace Solver {
void AlignRow(Matrix &row, const std::vector<Matrix *> &rest);
void Align(Matrix &A, Matrix *B = nullptr);
}; // namespace Solver