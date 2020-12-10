#include <fstream>
#include <iostream>
#include <stdexcept>

#include "matrix.h"
#include "solver.h"
#include "utils.h"

int main(int argc, char *argv[]) {
  if (argc < 4) throw std::runtime_error("Main error # 1");
  int n, m, k;
  Matrix A, x, B;

  n = std::stoi(argv[1]);
  m = std::stoi(argv[2]);
  k = std::stoi(argv[3]);

  if (k == 0) {
    if (argc != 4) throw std::runtime_error("Main error # 2");
    std::ifstream ifs(argv[4]);
    A = std::move(ReadMatrix(ifs, n));
  } else {
    A = std::move(Matrix(n, [k, n](int i, int j) { return f(k, n, i, j); }));
  }
  x = std::move(Matrix(1, n));
  B = std::move(Matrix(1, n));
  for (int i = 0; i < n; i += 2) B += A.col(i);

  Solver::Solve(A, B, x);
  double error = Solver::Discrepancy(A, B, x);

  for(int w : {1, 2, 4}){
    std::cerr << "workers = " << w << '\n';
    Solver::Async::Discrepancy(A, B, x, w);
  }

  std::cout << "Solution is x = {";
  bool first = true;
  for (int i = 0; i < m; ++i) {
    if (!first) std::cout << " ";
    first = false;
    std::cout << x.at(0, i);
  }
  std::cout << (m == n ? "}" : " ...}") << std::endl;
  std::cout << "Error is " << error << std::endl;
  return 0;
}