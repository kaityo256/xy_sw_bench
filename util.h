#pragma once
#include <cmath>
#include <mpi.h>
#include <vector>
class MyVector {
public:
  double x, y;
  MyVector(void) {
    x = 0.0;
    y = 0.0;
  }
  double abs(void) {
    return sqrt(x * x + y * y);
  }
  double arg(void) {
    return atan2(y, x);
  }
};

class SVariable {
private:
  std::vector<double> data;

public:
  double average;
  double binder;
  void add(double v) {
    data.push_back(v);
  }
  void calculate(void) {
    const int n = static_cast<int>(data.size());
    double s = 0.0;
    double s2 = 0.0;
    double s4 = 0.0;
    for (int i = 0; i < n; i++) {
      const double v = data[i];
      s += v;
      s2 += v * v;
      s4 += v * v * v * v;
    }
    average = s / static_cast<double>(n);
    s2 /= static_cast<double>(n);
    s4 /= static_cast<double>(n);
    binder = s4 / (s2 * s2);
    binder = 0.5 * (3.0 - binder);
  }
};

struct double_with_error {
  double average;
  double std_error;
  double_with_error(double v, double s) {
    average = v;
    std_error = s;
  };
  friend std::ostream &operator<<(std::ostream &os, const double_with_error &c) {
    os << c.average << " +- " << c.std_error;
    return os;
  };
};

double_with_error estimate_error(double value) {
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  double sum = 0;
  MPI_Allreduce(&value, &sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  value = value * value;
  double sum2 = 0;
  MPI_Allreduce(&value, &sum2, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  const double s = static_cast<double>(size);
  double ave = sum / s;
  double var = (sum2 - ave * ave * size) / (s - 1.0);
  var /= s;
  double_with_error a(ave, sqrt(var));
  return a;
}