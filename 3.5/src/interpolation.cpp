#include "interpolation.h"

#include <cassert>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <vector>

LinearInterpolation::LinearInterpolation(std::map<Arg, Val> &&data)
{
  for (auto it = data.begin(); it != std::prev(data.end()); ++it)
  {
    auto [x1, f1] = *it;
    auto [x2, f2] = *std::next(it);
    coefs[x2] = {f2, (f2 - f1) / (x2 - x1)};
  }
}

Val LinearInterpolation::operator()(Arg x) const
{
  auto it = coefs.upper_bound(x);
  if (it == coefs.end())
    --it;
  const auto &[xi, ci] = *it;
  auto sum = [val = Coef(1), step = x - xi](Val s, Coef C) mutable {s += C * val; val *= step; return s; };
  return std::accumulate(ci.begin(), ci.end(), Val(0), sum);
}

using Arr = std::vector<Coef>;
Arr solve_tridiag(const Arr &A, Arr B, const Arr &C, Arr F)
{
  int n = B.size() - 1;
  assert(A.size() == B.size() && B.size() == C.size() && C.size() == F.size());
  assert(A[0] == 0 && C[n] == 0);

  for (int i = 1; i < n + 1; ++i)
  {
    auto k = A[i] / B[i - 1];
    B[i] -= k * C[i - 1];
    F[i] -= k * F[i - 1];
  }

  F[n] /= B[n];
  for (int i = n - 1; i >= 0; --i)
    (F[i] -= C[i] * F[i + 1]) /= B[i];
  return F;
}

CubicInterpolation::CubicInterpolation(std::map<Arg, Val> &&data, Val d2_left, Val d2_right)
{
  Arr A(data.size(), 0);
  Arr B(data.size(), 0);
  Arr C(data.size(), 0);
  Arr F(data.size(), 0);

  B[0] = 1;
  F[0] = d2_left;
  int n = 1;
  for (auto it = std::next(data.begin()); it != std::prev(data.end()); ++it, ++n)
  {
    // (x_{i}-x_{i-1}) c_{i-1} + 2(x_{i+1}-x_{i-1}) c_{i} + (x_{i+1}-x_{i}) c_{i+1} =
    //        = 3 [(f_{i+1}-f_{i})/(x_{i+1}-x_{i}) - (f_{i}-f_{i-1})/(x_{i}-x_{i-1})]
    auto [x, f] = *it;
    auto [xn, fn] = *std::next(it);
    auto [xp, fp] = *std::prev(it);
    A[n] = x - xp;
    B[n] = Val(2) * (xn - xp);
    C[n] = xn - x;
    F[n] = Val(3) * ((fn - f) / C[n] - (f - fp) / A[n]);
  }
  B[n] = 1;
  F[n] = d2_right;

  Arr c2 = solve_tridiag(A, B, C, F);

  n = 1;
  for (auto it = std::next(data.begin()); it != data.end(); ++it, ++n)
  {
    auto [x, f] = *it;
    auto [xp, fp] = *std::prev(it);
    Arg d = x - xp;
    Coef c3 = (c2[n] - c2[n - 1]) / 3 / d;
    Coef c1 = (f - fp) / d + d * (2 * c2[n] + c2[n - 1]) / 3;
#ifdef DEBUG
    std::cout << "[" << xp << ", " << x << "], " << f << ", " << c1 << ", " << c2[n] << ", " << c3 << std::endl;
#endif
    coefs[x] = {f, c1, c2[n], c3};
  }
}

Val CubicInterpolation::operator()(Arg x) const
{
  auto it = coefs.upper_bound(x);
  if (it == coefs.end())
    --it;
  const auto &[xi, ci] = *it;
  auto sum = [val = Coef(1), step = x - xi](Val s, Coef C) mutable {s += C * val; val *= step; return s; };
  return std::accumulate(ci.begin(), ci.end(), Val(0), sum);
}

std::map<Arg, Val> collect_data(std::function<Val(Arg)> func, std::pair<Arg, Arg> boundaries, int N)
{
  std::map<Arg, Val> data;
  auto [x1, x2] = boundaries;
  float step = (x2 - x1) / N;
  for (int i = 0; i < N + 1; ++i)
  {
    auto x = x1 + step * i;
    data[x] = func(x);
  }
  return data;
}

std::function<Val(Arg)> create_linear(std::function<Val(Arg)> func, std::pair<Arg, Arg> boundaries, int N)
{
  std::map<Arg, Val> data = collect_data(func, boundaries, N);
  return any_to_func(LinearInterpolation(std::move(data)));
}

std::function<Val(Arg)> create_cubic(std::function<Val(Arg)> func, std::pair<Arg, Arg> boundaries, int N, Val d2_left, Val d2_right)
{
  std::map<Arg, Val> data = collect_data(func, boundaries, N);
  return any_to_func(CubicInterpolation(std::move(data), d2_left, d2_right));
}