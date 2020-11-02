#include "solver.h"
#include <cmath>
#include <iostream>
#include <utility>

constexpr double eps = 1e-10;

double length_hint(double x1, double x2, double &x1_sqr_len) {
  x1_sqr_len += x2 * x2;
  double len = sqrt(x1_sqr_len);
  return len;
}

void Solver::AlignRow(Matrix &col_A, const std::vector<Matrix *> &rest) {
  int N = col_A.rows();
  int i = 0;
  while (abs(col_A[{0, i}]) < eps && i < N)
    ++i;
  if (i == N)
    return; // throw std::runtime_error("Solver error # 1");
  col_A.swap(0, i, 'r');
  for (auto *item : rest)
    item->swap(i, 0, 'r');
  double &x1 = col_A[{0, 0}];
  double square_len = x1 * x1;
  for (int j = 1; j < N; ++j) {
    double &x2 = col_A[{0, j}];
    if (abs(x2) < eps)
      continue;
    double len = length_hint(x1, x2, square_len);
    double cos = x1 / len;
    double sin = -x2 / len;
    x1 = len;
    x2 = 0;
    for (auto *item : rest) {
      auto row0 = std::move(item->row(0));
      auto rowj = std::move(item->row(j));
      auto init_row0 = row0;
      (row0 *= cos).add_scaled(rowj, -sin);
      (rowj *= cos).add_scaled(init_row0, sin);
    }
  }
}

void Solver::Align(Matrix &A, Matrix *B) {
  int N = A.rows();
  int M = A.cols();
  for (int i = 0; i < N - 1; ++i) {
    auto subcol_A = std::move(A.submat({i, i}, {i, N - 1}));
    auto rest = std::move(A.submat({i + 1, i}, {M - 1, N - 1}));
    if (B) {
      auto subcol_B = std::move(B->submat({0, i}, {0, N - 1}));
      AlignRow(subcol_A, {&rest, &subcol_B});
    } else {
      AlignRow(subcol_A, {&rest});
    }
  }
}

void Solver::QuasiTriangulate(Matrix &A, Matrix *B) {
  int N = A.rows();
  auto trimA = std::move(A.submat({0, 1}, {N - 1, N - 1}));
  if (B) {
    auto trimB = std::move(B->submat({0, 1}, {0, N - 1}));
    Solver::Align(trimA, &trimB);
  } else {
    Solver::Align(trimA);
  }
}

Matrix Solver::DecomposeLR(Matrix &A) {
  Solver::QuasiTriangulate(A);
  int N = A.rows();
  Matrix LR = Matrix(N);
  for (int i = 0; i < N; ++i)
    LR[{i, 0}] = A[{i, 0}];
  for (int i = 1; i < N; ++i) {
    double &l = LR[{i - 1, i}];
    l = A[{i - 1, i}] / LR[{i - 1, i - 1}];
    for (int k = i; k < N; ++k)
      LR[{k, i}] = A[{k, i}] - l * LR[{k, i - 1}];
  }

  return LR;
}
