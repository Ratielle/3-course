#include <random>

#include "matrix.h"
#include "solver.h"
#include "test_runner.h"

std::ostream &operator<<(std::ostream &os, const MatrixSize &s) {
  return os << "(" << s.col << ", " << s.row << ")";
}

namespace Test_Matrix {
double get_unit(int, int) { return 1; }

void Constructor() {
  int col = 10;
  int row = 20;
  {
    int n = col;
    Matrix A(n);
    ASSERT_EQUAL(A.cols(), n);
    ASSERT_EQUAL(A.rows(), n);
    for (int i = 0; i < n; ++i)
      for (int j = 0; j < n; ++j) ASSERT_EQUAL(0.0, A.at(i, j));
  }

  {
    Matrix A(col, row);
    ASSERT_EQUAL(A.cols(), col);
    ASSERT_EQUAL(A.rows(), row);
    for (int i = 0; i < col; ++i)
      for (int j = 0; j < row; ++j) ASSERT_EQUAL(0.0, A.at(i, j));
  }

  {
    int n = col;
    Matrix A(n, get_unit);
    ASSERT_EQUAL(A.cols(), n);
    ASSERT_EQUAL(A.rows(), n);
    for (int i = 0; i < n; ++i)
      for (int j = 0; j < n; ++j) ASSERT_EQUAL(1.0, A.at(i, j));
  }

  {
    const int len = col * row;
    double *arr = new double[len];
    Matrix A(col, row, arr);
    ASSERT_EQUAL(A.cols(), col);
    ASSERT_EQUAL(A.rows(), row);
    ASSERT_EQUAL(arr, &A.at(0, 0));
  }

  {
    const int len = col * row;
    double *arr = new double[len];
    for (int i = 0; i < len; ++i) arr[i] = i;
    Matrix A(col, row, arr);
    Matrix B(A);
    ASSERT_EQUAL(A.size(), B.size());
    for (int i = 0; i < col; ++i)
      for (int j = 0; j < row; ++j) ASSERT_EQUAL(B.at(i, j), A.at(i, j));
  }
}

void SumSubtract() {
  const int len = 40;
  double input_A[len];
  double input_B[len];

  for (int i = 0; i < len; ++i) {
    input_A[i] = i;
    input_B[len - i - 1] = i;
  }

  int col = 8;
  int row = len / col;
  {
    Matrix A(col, row, input_A);
    A.release();
    Matrix B(col, row, input_B);
    B.release();
    Matrix C = A + B;
    for (int i = 0; i < col; ++i)
      for (int j = 0; j < row; ++j) ASSERT_EQUAL(C.at(i, j), len - 1);
  }
  {
    Matrix A(col, row, input_A);
    A.release();
    Matrix B(col, row, input_A);
    B.release();
    Matrix C = A - B;
    for (int i = 0; i < col; ++i)
      for (int j = 0; j < row; ++j) ASSERT_EQUAL(C.at(i, j), 0.0);
  }
}

void Muliply() {
  const int len = 40;
  double input_A[len]{-6, -6, 5,  -7, 10, 8, -1, -2, 0,  1,  10, 4,  -6, -5,
                      0,  1,  -2, -1, -1, 9, -1, -6, 7,  0,  -7, -2, 8,  5,
                      0,  0,  4,  7,  2,  4, 9,  -6, -2, -9, -4, -5};
  double input_B[len]{-1,  8, -8, 1,  -5, -2, -1, -1, 10, -2, -4,  -3, -7, 6,
                      5,   7, 4,  9,  -1, -8, 5,  -7, -1, 9,  -10, -6, 8,  -10,
                      -10, 6, 5,  -3, 0,  -2, -7, 2,  8,  -6, 5,   7};

  double output[] = {-58, -104, -122, -27,  64,  -12,  -5, 15,  67,
                     53,  137,  -38,  166,  10,  -140, 48, -14, 5,
                     43,  60,   -74,  -125, -15, 157,  34};

  int row = 5;
  int col = len / row;
  {
    int n = row;
    Matrix A(col, row, input_A);
    A.release();
    Matrix B(row, col, input_B);
    B.release();
    Matrix C = A * B;
    ASSERT_EQUAL(C.cols(), n);
    ASSERT_EQUAL(C.rows(), n);
    for (int i = 0; i < n; ++i)
      for (int j = 0; j < n; ++j) ASSERT_EQUAL(C.at(i, j), output[j * n + i]);
  }

  {
    double scale = 4.0;
    Matrix A(col, row, get_unit);
    A *= scale;

    for (int i = 0; i < col; ++i)
      for (int j = 0; j < row; ++j) ASSERT_EQUAL(A.at(i, j), scale);
  }
}

void Other() {
  {
    int n = 8;

    Matrix A(n, get_unit);
    ASSERT_EQUAL(A.norm(), double(n));
  }

  {
    int n = 8;
    int i = 3, j = 7;
    double scale_i = 2, scale_j = 4;
    Matrix A(n, get_unit);
    A.row(i) *= scale_i;
    A.row(j) *= scale_j;

    A.swap(i, j, 'r');

    for (int k = 0; k < n; ++k) {
      ASSERT_EQUAL(A.at(k, i), scale_j);
      ASSERT_EQUAL(A.at(k, j), scale_i);
    }
  }

  {
    int n = 8;
    int i = 3, j = 7;
    double scale_i = 2, scale_j = 4;
    Matrix A(n, get_unit);
    A.col(i) *= scale_i;
    A.col(j) *= scale_j;

    A.swap(i, j, 'c');

    for (int k = 0; k < n; ++k) {
      ASSERT_EQUAL(A.at(i, k), scale_j);
      ASSERT_EQUAL(A.at(j, k), scale_i);
    }
  }

  {
    int n = 8;
    double max = 1;
    Index pos{.col = 3, .row = 5};
    Matrix A(n);
    A[pos] = max;
    auto result = A.max_element();
    ASSERT_EQUAL(result.first, pos);
    ASSERT_EQUAL(result.second, max);
  }
}
}  // namespace Test_Matrix

namespace Test_Solver {
void Direct() {
  int n = 3;
  double input_A[] = {3, 2, -5, 2, -1, 3, 1, 2, -1};
  double input_B[] = {-1, 13, 9};
  double output_A[] = {1, -3. / 5, -2. / 5, 0, 1, 1. / 19, 0, 0, 1};
  double output_B[] = {1. / 5, 62. / 19, 5};

  Matrix A(n, n, input_A);
  A.release();
  Matrix B(1, n, input_B);
  B.release();
  Matrix x(1, n);

  Solver::Direct(A, B, x);

  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j) ASSERT_EQUAL(A.at(i, j), output_A[j * n + i]);
  for (int i = 0; i < n; ++i) ASSERT_EQUAL(B.at(0, i), output_B[i]);
}

void Reverse() {
  int n = 3;
  double input_A[] = {1, -3. / 5, -2. / 5, 0, 1, 1. / 19, 0, 0, 1};
  double input_B[] = {1. / 5, 62. / 19, 5};
  double output_A[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
  double output_B[] = {4, 3, 5};

  Matrix A(n, n, input_A);
  A.release();
  Matrix B(1, n, input_B);
  B.release();

  Solver::Reverse(A, B);

  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j) ASSERT_EQUAL(A.at(i, j), output_A[j * n + i]);
  for (int i = 0; i < n; ++i) ASSERT_EQUAL(B.at(0, i), output_B[i]);
}

void Solve() {
  int n = 3;
  double input_A[] = {3, 2, -5, 2, -1, 3, 1, 2, -1};
  double input_B[] = {-1, 13, 9};
  double output_x[] = {5, 4, 3};

  Matrix A(n, n, input_A);
  A.release();
  Matrix B(1, n, input_B);
  B.release();
  Matrix x(1, n);

  Solver::Solve(A, B, x);

  for (int i = 0; i < n; ++i) ASSERT_EQUAL(x.at(0, i), output_x[i]);
}
}  // namespace Test_Solver

int main() {
  TestRunner tr;
  RUN_TEST(tr, Test_Matrix::Constructor);
  RUN_TEST(tr, Test_Matrix::SumSubtract);
  RUN_TEST(tr, Test_Matrix::Muliply);
  RUN_TEST(tr, Test_Matrix::Other);
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  RUN_TEST(tr, Test_Solver::Direct);
  RUN_TEST(tr, Test_Solver::Reverse);
  RUN_TEST(tr, Test_Solver::Solve);
  return 0;
}