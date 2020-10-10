#pragma once
#include <istream>
#include <string>

#include "matrix.h"

double f(int k, int n, int i, int j);
Matrix ReadMatrix(std::istream &is, int n);