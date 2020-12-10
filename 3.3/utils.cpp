#include "utils.h"

#include <stdexcept>

double f(int k, int n, int i, int j) {
  switch (k) {
    case 1:
      return n - std::max(i, j) + 1;
    case 2:
      return std::max(i, j);
    case 3:
      return std::abs(i - j);
    case 4:
      return 1. / (i + j + 1);
    default:
      throw std::runtime_error("Utils error # 1");
  }
}

Matrix ReadMatrix(std::istream &is, int n) {
  double *data = new double[n * n];
  for (int i = 0; i < n * n; ++i) {
    if (!is.good()) throw std::runtime_error("Utils error # 2");
    is >> data[i];
  }
  return Matrix(n, n, data);
}
