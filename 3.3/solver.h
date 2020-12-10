#pragma once
#include "matrix.h"

namespace Solver {
void Direct(Matrix &A, Matrix &b, Matrix &x);
void Reverse(Matrix &A, Matrix &b);
void Solve(const Matrix &A, const Matrix &b, Matrix &x);
double Discrepancy(const Matrix &A, const Matrix &b, const Matrix &x);
namespace Async {
double Discrepancy(const Matrix &A, const Matrix &b, const Matrix &x,
                   int workers);
} // namespace Async
} // namespace Solver