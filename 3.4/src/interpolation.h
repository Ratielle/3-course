#pragma once
#include "types.h"

#include <array>
#include <functional>
#include <map>

class LinearInterpolation
{
public:
  LinearInterpolation(std::map<Arg, Val> &&data);
  Val operator()(Arg x) const;

private:
  std::map<Arg, std::array<Coef, 2>> coefs;
};

class CubicInterpolation
{
public:
  CubicInterpolation(std::map<Arg, Val> &&data, Val d2_left, Val d2_right);
  Val operator()(Arg x) const;

private:
  std::map<Arg, std::array<Coef, 4>> coefs;
};

std::map<Arg, Val> collect_data(std::function<Val(Arg)> func, std::pair<Arg, Arg> boundaries, int N);

std::function<Val(Arg)> create_linear(std::function<Val(Arg)> func, std::pair<Arg, Arg> boundaries, int N);
std::function<Val(Arg)> create_cubic(std::function<Val(Arg)> func, std::pair<Arg, Arg> boundaries, int N, Val d2_left, Val d2_right);

template <class Callable>
std::function<Val(Arg)> any_to_func(Callable &&c)
{
  auto func = [f{std::move(c)}](Arg x) -> Val { return f(x); };
  return func;
}

template <class Callable>
std::function<Val(Arg)> any_to_func(const Callable &c)
{
  auto func = [f = c](Arg x) -> Val { return f(x); };
  return func;
}