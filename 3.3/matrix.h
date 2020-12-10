#pragma once
#include <functional>
#include <ostream>
#include <utility>
// x-----> cols
// |
// |
// v
// rows

struct Index {
  int col;
  int row;
  bool operator==(const Index &other) const;
};

using MatrixSize = Index;
using Initializer = std::function<double(int, int)>;
using Comparator = std::function<bool(double, double)>;

class Matrix {
 public:
  // Constructors
  Matrix();
  Matrix(int N);
  Matrix(int cols, int rows);

  Matrix(int N, Initializer func);
  Matrix(int cols, int rows, Initializer func);

  Matrix(int cols, int rows, double *data);

  Matrix(const Matrix &other);
  Matrix(Matrix &&other);
  // Destructor
  ~Matrix();
  // Getters
  int cols() const;
  int rows() const;
  MatrixSize size() const;

  const Matrix row(int i) const;
  Matrix row(int i);
  const Matrix col(int i) const;
  Matrix col(int i);
  Matrix submat(const Index &i1, const Index &i2);
  const Matrix submat(const Index &i1, const Index &i2) const;

  double &at(int col, int row);
  double at(int col, int row) const;
  double &operator[](Index i);
  double operator[](Index i) const;

  std::pair<Index, double> max_element(Comparator less = nullptr) const;
  // Operators
  Matrix &operator+=(const Matrix &other);
  Matrix &add_scaled(const Matrix &other, double scale);
  Matrix multiply_async(const Matrix &other, int workers = 1) const;
  Matrix operator+(const Matrix &other) const;
  Matrix &operator-=(const Matrix &other);
  Matrix operator-(const Matrix &other) const;
  Matrix &operator=(const Matrix &other);
  Matrix &operator=(Matrix &&other);
  Matrix &operator*=(double scale);
  Matrix operator*(const Matrix &other) const;
  void swap(int i, int j, char what);
  double norm() const;
  void release();

 private:
  Matrix(int cols, int rows, int step);
  static void multiply(const Matrix &A, const Matrix &B, Matrix &C);
  static Matrix submat(const Index &i1, const Index &i2, const Matrix &M);
  double *_data;
  int _cols;
  int _rows;
  int _step;
  bool _reference{false};
};

std::ostream &operator<<(std::ostream &os, const Matrix &M);