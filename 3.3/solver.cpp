#include "solver.h"

#include <iostream>

#include "profiler.h"

void Solver::Direct(Matrix &A, Matrix &B, Matrix &x) {
  LOG_DURATION("Algorithm direct step time");
  int n = B.rows();
  for (int i = 0; i < n; ++i) x[{0, i}] = i;
  for (int i = 0; i < n; ++i) {
    auto max =
        A.submat({i, i}, {n - 1, n - 1}).max_element([](double a, double b) {
          return std::abs(a) < std::abs(b);
        });
    if (std::abs(max.second) < 1e-14)
      throw std::runtime_error("Solver error # 1");

    A.swap(i, i + max.first.row, 'r');
    B.swap(i, i + max.first.row, 'r');

    A.swap(i, i + max.first.col, 'c');
    x.swap(i, i + max.first.col, 'r');

    double scale = 1. / max.second;
    A.row(i) *= scale;
    B.row(i) *= scale;
    for (int j = i + 1; j < n; ++j) {
      B.row(j).add_scaled(B.row(i), -A[{i, j}]);
      A.submat({i, j}, {n - 1, j})
          .add_scaled(A.submat({i, i}, {n - 1, i}), -A[{i, j}]);
    }
  }
}

void Solver::Reverse(Matrix &A, Matrix &B) {
  LOG_DURATION("Algorithm reverse step time");
  int n = B.rows();
  for (int i = n - 1; i > 0; --i)
    for (int j = 0; j < i; ++j) {
      B.row(j).add_scaled(B.row(i), -A[{i, j}]);
      A[{i, j}] = 0.0;
    }
}

void Solver::Solve(const Matrix &A, const Matrix &B, Matrix &x) {
  int n = x.rows();
  if (B.rows() != n || A.cols() != n || A.rows() != n)
    throw std::runtime_error("Solver error # 2");

  Matrix _A(A);
  Matrix _B(B);
  LOG_DURATION("Algorithm full time");
  Direct(_A, _B, x);
  Reverse(_A, _B);
  for (int i = 0; i < x.rows(); ++i) {
    int j = static_cast<int>(x[{0, i}]);
    while (i != j) {
      x.swap(i, j, 'r');
      _B.swap(i, j, 'r');
      j = static_cast<int>(x[{0, i}]);
    }
  }
  x = std::move(_B);
}

double Solver::Discrepancy(const Matrix &A, const Matrix &B, const Matrix &x) {
  LOG_DURATION("Error calculation time");
  Matrix result = A * x;
  result -= B;
  return result.norm();
}