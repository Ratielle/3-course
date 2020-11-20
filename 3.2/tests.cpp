#include "matrix.h"
#include "solver.h"
#include "test_runner.h"

#include <cmath>

std::ostream &operator<<(std::ostream &os, const MatrixSize &s) {
  return os << "(" << s.col << ", " << s.row << ")";
}

namespace Test_Solver {
void AlignRow() {
  int n = 4;
  double input_A[] = {1, 5, 3, 2};
  Matrix A(1, n, input_A);
  A.release();
  {
    Matrix A1 = A;
    Solver::AlignRow(A1, {});
    ASSERT_EQUAL(A1.at(0, 0), A.norm());
    for (int j = 1; j < n; ++j)
      ASSERT_EQUAL(A1.at(0, j), 0.0);
  }
  {
    Matrix A1 = A;
    Matrix A2 = A;
    Solver::AlignRow(A1, {&A2});
    for (int j = 0; j < n; ++j)
      ASSERT_EQUAL(A1.at(0, j), A2.at(0, j));
  }
  {
    Matrix A1 = A;
    Matrix A2 = A;
    A2.swap(0, 2, 'r');
    A2.swap(1, 3, 'r');
    Matrix A3 = A2;
    Solver::AlignRow(A1, {&A2, &A3});
    for (int j = 0; j < n; ++j)
      ASSERT_EQUAL(A3.at(0, j), A3.at(0, j));
  }
}

void Align() {
  int n = 3;
  double input_A[] = {3, 2, -5, 2, -1, 3, 1, 2, -1};
  double input_B[] = {-1, 13, 9};
  double output_A[] = {3.741657386773941,
                       1.6035674514745462,
                       -2.672612419124244,
                       0,
                       2.5354627641855494,
                       -4.225771273642583,
                       0,
                       0,
                       -3.1622776601683795};
  double output_B[] = {8.55235974119758, -4.225771273642583,
                       -12.649110640673516};

  Matrix A(n, n, input_A);
  A.release();
  Matrix B(1, n, input_B);
  B.release();
  Matrix x(1, n);
  Solver::Align(A, &B);
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j)
      ASSERT_EQUAL(A.at(i, j), output_A[j * n + i]);
  for (int i = 0; i < n; ++i)
    ASSERT_EQUAL(B.at(0, i), output_B[i]);
}

void QuasiTriangulate() {
  int n = 3;
  double input_A[] = {3, 2, -5, 2, -1, 3, 1, 2, -1};
  double s5 = sqrt(5);
  double output_A[] = {3, 2, -5, s5, 0, s5, 0, s5, -s5};
  Matrix A(n, n, input_A);
  A.release();
  Solver::QuasiTriangulate(A);
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j)
      ASSERT_EQUAL(A.at(i, j), output_A[j * n + i]);
}

void Decompose() {
  int n = 3;
  double input_A[] = {3, 2, -5, 2, -1, 3, 1, 2, -1};
  Matrix A(n, n, input_A);
  A.release();
  auto LR = Solver::DecomposeLR(A);
  Matrix L(n, [&](int i, int j) { return i < j ? LR[{i, j}] : i == j; });
  Matrix R(n, [&](int i, int j) { return i >= j ? LR[{i, j}] : 0.0; });
  Matrix A2 = L * R;

  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j)
      ASSERT_EQUAL(A.at(i, j), A2.at(i, j));
}

void EigenValues() {
  int n = 3;
  {
    double input_A[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
    double output_v[] = {1, 1, 1};
    Matrix A(n, n, input_A);
    A.release();
    auto v = Solver::EigenValues(A);
    for (int i = 0; i < n; ++i)
      ASSERT_EQUAL(v[i], output_v[i]);
  }
  {
    double input_A[] = {5, 6, 3, -1, 0, 1, 1, 2, -1};
    double output_v[] = {4, 2, -2};
    Matrix A(n, n, input_A);
    A.release();
    auto v = Solver::EigenValues(A);
    for (int i = 0; i < n; ++i)
      ASSERT_EQUAL(v[i], output_v[i]);
  }
}
} // namespace Test_Solver

int main() {
  TestRunner tr;
  RUN_TEST(tr, Test_Solver::AlignRow);
  RUN_TEST(tr, Test_Solver::Align);
  RUN_TEST(tr, Test_Solver::QuasiTriangulate);
  RUN_TEST(tr, Test_Solver::Decompose);
  RUN_TEST(tr, Test_Solver::EigenValues);
  return 0;
}