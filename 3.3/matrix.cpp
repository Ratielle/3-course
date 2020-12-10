#include "matrix.h"

#include <cmath>
#include <future>
#include <stdexcept>
#include <vector>

bool Index::operator==(const Index &other) const {
  return col == other.col && row == other.row;
}

Matrix::Matrix()
    : _cols(0), _rows(0), _step(0), _data(nullptr), _reference(true) {}

Matrix::Matrix(int cols, int rows, int step)
    : _cols(cols), _rows(rows), _step(step) {
  if (cols < 1 || rows < 1) throw std::domain_error("Matix error # 1");
}

Matrix::Matrix(int N) : Matrix(N, N, N) { _data = new double[N * N](); }

Matrix::Matrix(int cols, int rows) : Matrix(cols, rows, cols) {
  _data = new double[cols * rows]();
}

Matrix::Matrix(int N, Initializer func) : Matrix(N, N, N) {
  _data = new double[N * N];
  double *p = _data;
  for (int j = 0; j < _rows; ++j) {
    for (int i = 0; i < _cols; ++i) p[i] = func(i, j);
    p += _step;
  }
}

Matrix::Matrix(int cols, int rows, Initializer func)
    : Matrix(cols, rows, cols) {
  _data = new double[cols * rows];
  double *p = _data;
  for (int j = 0; j < _rows; ++j) {
    for (int i = 0; i < _cols; ++i) p[i] = func(i, j);
    p += _step;
  }
}

Matrix::Matrix(int cols, int rows, double *data) : Matrix(cols, rows, cols) {
  _data = data;
}

Matrix::Matrix(const Matrix &other)
    : Matrix(other._cols, other._rows, other._cols) {
  _data = new double[other._cols * other._rows];
  double *pt = _data;
  double *po = other._data;
  for (int j = 0; j < _rows; ++j) {
    for (int i = 0; i < _cols; ++i) pt[i] = po[i];
    pt += _step;
    po += other._step;
  }
}

Matrix::Matrix(Matrix &&other) : Matrix(other._cols, other._rows, other._step) {
  _data = other._data;
  _reference = other._reference;
  other._data = nullptr;
  other._cols = other._rows = other._step = 0;
}

Matrix::~Matrix() {
  if (!_reference) delete _data;
}

int Matrix::cols() const { return _cols; }
int Matrix::rows() const { return _rows; }
MatrixSize Matrix::size() const { return {_cols, _rows}; }

Matrix Matrix::row(int i) { return submat({0, i}, {_cols - 1, i}); }
const Matrix Matrix::row(int i) const { return submat({0, i}, {_cols - 1, i}); }
Matrix Matrix::col(int i) { return submat({i, 0}, {i, _rows - 1}); }
const Matrix Matrix::col(int i) const { return submat({i, 0}, {i, _rows - 1}); }

Matrix Matrix::submat(const Index &i1, const Index &i2, const Matrix &M) {
  if (i1.col < 0 || i2.col < 0 || i1.row < 0 || i2.row < 0)
    throw std::range_error("Matix error # 2");
  if (i1.col >= M._cols || i2.col >= M._cols)
    throw std::range_error("Matix error # 3");
  if (i1.row >= M._rows || i2.row >= M._rows)
    throw std::range_error("Matix error # 4");

  Index left_top = {std::min(i1.col, i2.col), std::min(i1.row, i2.row)};
  Index right_bottom = {std::max(i1.col, i2.col), std::max(i1.row, i2.row)};
  double *p = M._data + left_top.col + left_top.row * M._step;
  Matrix s(right_bottom.col - left_top.col + 1,
           right_bottom.row - left_top.row + 1, p);
  s._reference = true;
  s._step = M._cols;
  return s;
}
Matrix Matrix::submat(const Index &i1, const Index &i2) {
  return submat(i1, i2, *this);
}
const Matrix Matrix::submat(const Index &i1, const Index &i2) const {
  return submat(i1, i2, *this);
}

double &Matrix::at(int col, int row) {
  if (col < 0 || col >= _cols) throw std::range_error("Matix error # 5");
  if (row < 0 || row >= _rows) throw std::range_error("Matix error # 6");
  return _data[row * _step + col];
}

double Matrix::at(int col, int row) const {
  if (col < 0 || col >= _cols) throw std::range_error("Matix error # 7");
  if (row < 0 || row >= _rows) throw std::range_error("Matix error # 8");
  return _data[row * _step + col];
}
double &Matrix::operator[](Index i) { return _data[i.row * _step + i.col]; }
double Matrix::operator[](Index i) const {
  return _data[i.row * _step + i.col];
}

std::pair<Index, double> Matrix::max_element(Comparator less) const {
  if (!less) less = [](double a, double b) { return a < b; };

  Index max_index = {0, 0};
  double max_elem = *_data;
  double *p = _data;
  for (int j = 0; j < _rows; ++j) {
    for (int i = 0; i < _cols; ++i)
      if (less(max_elem, p[i])) {
        max_index = {i, j};
        max_elem = p[i];
      }
    p += _step;
  }

  return {max_index, max_elem};
}

Matrix &Matrix::operator+=(const Matrix &other) {
  if (!(size() == other.size())) throw std::domain_error("Matix error # 9");
  double *pt = _data;
  double *po = other._data;
  for (int j = 0; j < _rows; ++j) {
    for (int i = 0; i < _cols; ++i) pt[i] += po[i];
    pt += _step;
    po += other._step;
  }
  return *this;
}

Matrix &Matrix::add_scaled(const Matrix &other, double scale) {
  if (!(size() == other.size())) throw std::domain_error("Matix error # 10");
  double *pt = _data;
  double *po = other._data;
  for (int j = 0; j < _rows; ++j) {
    for (int i = 0; i < _cols; ++i) pt[i] += scale * po[i];
    pt += _step;
    po += other._step;
  }
  return *this;
}

Matrix Matrix::operator+(const Matrix &other) const {
  Matrix result(*this);
  result += other;
  return result;
}

Matrix &Matrix::operator-=(const Matrix &other) {
  if (!(size() == other.size())) throw std::domain_error("Matix error # 11");
  double *pt = _data;
  double *po = other._data;
  for (int j = 0; j < _rows; ++j) {
    for (int i = 0; i < _cols; ++i) pt[i] -= po[i];
    pt += _step;
    po += other._step;
  }
  return *this;
}

Matrix Matrix::operator-(const Matrix &other) const {
  Matrix result(*this);
  result -= other;
  return result;
}

Matrix &Matrix::operator=(const Matrix &other) {
  if (this != &other) {
    if (_cols * _rows != other._cols * other._rows) {
      delete _data;
      _data = new double[other._cols * other._rows];
    }
    double *pt = _data;
    double *po = other._data;
    for (int j = 0; j < _rows; ++j) {
      for (int i = 0; i < _cols; ++i) pt[i] = po[i];
      pt += _step;
      po += other._step;
    }
    _cols = other._cols;
    _rows = other._rows;
    _step = _cols;
    _reference = false;
  }
  return *this;
}

Matrix &Matrix::operator=(Matrix &&other) {
  if (this != &other) {
    if (!_reference)
      delete _data;
    _cols = other._cols;
    _rows = other._rows;
    _step = other._step;
    _reference = other._reference;
    _data = other._data;
    other._data = nullptr;
    other._cols = other._rows = other._step = 0;
  }
  return *this;
}

Matrix &Matrix::operator*=(double scale) {
  double *p = _data;
  for (int j = 0; j < _rows; ++j) {
    for (int i = 0; i < _cols; ++i) p[i] *= scale;
    p += _step;
  }
  return *this;
}

void Matrix::multiply(const Matrix &A, const Matrix &B, Matrix &C) {
  double *pt = A._data;
  double *pr = C._data;
  for (int j = 0; j < A._rows; ++j) {
    for (int i = 0; i < B._cols; ++i) {
      double *po = B._data + i;
      for (int k = 0; k < A._cols; ++k) {
        pr[i] += *po * pt[k];
        po += B._step;
      }
    }
    pt += A._step;
    pr += C._step;
  }
}

Matrix Matrix::operator*(const Matrix &other) const {
  if (_cols != other._rows)
    throw std::domain_error("Matix error # 12");
  Matrix result(other._cols, _rows);
  multiply(*this, other, result);

  return result;
}

void Matrix::swap(int i, int j, char what) {
  if (i < 0 || j < 0) throw std::domain_error("Matix error # 13");
  if (i == j) return;
  double tmp;
  double *pi, *pj;
  switch (what) {
    case 'c':
      if (i >= _cols || j >= _cols) throw std::range_error("Matix error # 14");
      pi = _data + i;
      pj = _data + j;
      for (int k = 0; k < _rows; ++k) {
        tmp = pi[k * _step];
        pi[k * _step] = pj[k * _step];
        pj[k * _step] = tmp;
      }
      return;
    case 'r':
      if (i >= _rows || j >= _rows) throw std::range_error("Matix error # 15");
      pi = _data + i * _step;
      pj = _data + j * _step;
      for (int k = 0; k < _cols; ++k) {
        tmp = pi[k];
        pi[k] = pj[k];
        pj[k] = tmp;
      }
      return;
    default:
      throw std::runtime_error("Matix error # 16");
  }
}

double Matrix::norm() const {
  double sum = 0;
  double *p = _data;
  for (int j = 0; j < _rows; ++j) {
    for (int i = 0; i < _cols; ++i) sum += p[i] * p[i];
    p += _step;
  }

  return sqrt(sum);
}

void Matrix::release() { _reference = true; }

std::ostream &operator<<(std::ostream &os, const Matrix &M) {
  for (int j = 0; j < M.rows(); ++j) {
    for (int i = 0; i < M.cols(); ++i) os << M[{i, j}] << ' ';
    os << '\n';
  }
  return os;
}

Matrix Matrix::multiply_async(const Matrix &other, int workers) const {
  if (workers < 1)
    throw std::runtime_error("Matix error # 17");
  if (_cols != other._rows)
    throw std::domain_error("Matix error # 18");
  const int N = _rows;
  const int M = other._cols;
  const int K = _cols;
  const int step = std::max(N / workers + N % workers, 1);
  Matrix result(M, N);

  std::vector<std::future<void>> futures;
  futures.reserve(workers);
  int i = 0;
  for (; i < N - step; i += step) {
    auto f = [&](int i) {
      auto subresult = std::move(result.submat({0, i}, {M - 1, i + step - 1}));
      const auto subthis = submat({0, i}, {K - 1, i + step - 1});
      multiply(subthis, other, subresult);
    };
    futures.push_back(std::async(f, i));
  }

  auto subresult = std::move(result.submat({0, i}, {M - 1, N - 1}));
  multiply(submat({0, i}, {K - 1, N - 1}), other, subresult);

  for (const auto &f : futures)
    f.wait();

  return result;
}