#include "mpistream.h"
#include "util.h"
#include <algorithm>
#include <iostream>
#include <math.h>
#include <mpi.h>
#include <random>
#include <vector>
const int L = 32;
const int N = L * L * L;
const double Pi = M_PI;
const char *version = "1.0";

int cluster[N];
int flip[N];
int neighbors[N][6];
double sx[N], sy[N];

std::mt19937 mt;

double myrand(void) {
  std::uniform_real_distribution<> ud(0.0, 1.0);
  return ud(mt);
}

void init(void) {
  for (int i = 0; i < N; i++) {
    sx[i] = 1.0;
    sy[i] = 0.0;
  }
}
int get_cluster_number(int index) {
  int i = index;
  while (i != cluster[i]) {
    i = cluster[i];
  }
  return i;
}
void connect_bond(const int i1, int i2, const double r, const double beta) {
  const double sx1 = sx[i1];
  const double sy1 = sy[i1];
  const double sx2 = sx[i2];
  const double sy2 = sy[i2];
  const double dE = sx1 * sx2 + sy1 * sy2 - (sx1 * sx2 - sy1 * sy2) * cos(2.0 * r) - (sy1 * sx2 + sx1 * sy2) * sin(2.0 * r);
  if (myrand() > 1.0 - exp(-beta * dE)) {
    return;
  }
  const int c1 = get_cluster_number(i1);
  const int c2 = get_cluster_number(i2);
  if (c1 < c2) {
    cluster[c2] = c1;
  } else {
    cluster[c1] = c2;
  }
}

void cluster_flip(double beta) {
  for (int i = 0; i < N; i++) {
    cluster[i] = i;
    flip[i] = (myrand() > 0.5);
  }
  const double r = 2.0 * Pi * static_cast<double>(myrand());

  for (int i = 0; i < N; i++) {
    connect_bond(i, neighbors[i][1], r, beta);
    connect_bond(i, neighbors[i][3], r, beta);
    connect_bond(i, neighbors[i][5], r, beta);
  }
  for (int i = 0; i < N; i++) {
    const int c = get_cluster_number(i);
    if (flip[c]) {
      double s = atan2(sy[i], sx[i]);
      s = 2.0 * r - s;
      sx[i] = cos(s);
      sy[i] = sin(s);
    }
  }
}

void mc_onestep(double beta) {
  cluster_flip(beta);
}

MyVector magnetization(void) {
  MyVector m;
  for (int i = 0; i < N; i++) {
    m.x += sx[i];
    m.y += sy[i];
  }
  m.x /= static_cast<double>(N);
  m.y /= static_cast<double>(N);
  return m;
}

double domc_beta(double beta, const int T_LOOP, const int O_LOOP) {
  init();
  for (int j = 0; j < T_LOOP; j++) {
    mc_onestep(beta);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  double start_time = MPI_Wtime();
  SVariable s_m10;
  for (int j = 0; j < O_LOOP; j++) {
    mc_onestep(beta);
    MyVector m = magnetization();
    const double m1 = m.abs();
    const double s = m.arg();
    s_m10.add(m1);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  double end_time = MPI_Wtime();
  s_m10.calculate();
  double_with_error d_m10 = estimate_error(s_m10.average);
  double_with_error d_U10 = estimate_error(s_m10.binder);
  mout << "T = " << 1.0 / beta << std::endl;
  mout << "Magnetization = " << d_m10 << std::endl;
  mout << "Binder Ratio = " << d_U10 << std::endl;
  mout << std::endl;
  return end_time - start_time;
}

int pos2index(int ix, int iy, int iz) {
  if (ix < 0) ix += L;
  if (ix >= L) ix -= L;
  if (iy < 0) iy += L;
  if (iy >= L) iy -= L;
  if (iz < 0) iz += L;
  if (iz >= L) iz -= L;
  return ix + iy * L + iz * L * L;
}

void init_neighbor(void) {
  for (int i = 0; i < N; i++) {
    const int ix = i % L;
    const int iy = (i / L) % L;
    const int iz = (i / (L * L));
    neighbors[i][0] = pos2index(ix - 1, iy, iz);
    neighbors[i][1] = pos2index(ix + 1, iy, iz);
    neighbors[i][2] = pos2index(ix, iy - 1, iz);
    neighbors[i][3] = pos2index(ix, iy + 1, iz);
    neighbors[i][4] = pos2index(ix, iy, iz - 1);
    neighbors[i][5] = pos2index(ix, iy, iz + 1);
  }
}

int main(int argc, char **argv) {
  const int T_LOOP = 1000;   // Thermalization loop
  const int O_LOOP = 153600; // Number of samples
  MPI_Init(&argc, &argv);
  int rank, procs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &procs);
  mt.seed(rank);
  mout.SetRank(rank);
  init_neighbor();
  const double Tc = 2.2016;
  mout << "MC benchmark: Version " << version << std::endl;
  mout << "L = " << L << std::endl;
  mout << "Procs = " << procs << std::endl;
  mout << "Thermalize Loop = " << T_LOOP << std::endl;
  mout << "Total Samples = " << O_LOOP << std::endl;
  const int observe_loop = O_LOOP / procs;
  mout << "Observe Loops = " << observe_loop << std::endl;
  double elapse = domc_beta(1.0 / Tc, T_LOOP, observe_loop);
  mout << "Elapsed Time = " << elapse << std::endl;
  double mcs = static_cast<double>(observe_loop) / elapse;
  mout << "MCs/sec/procs = " << mcs << std::endl;
  mout << "Total MCs/sec = " << mcs * procs << std::endl;
  MPI_Finalize();
}
//----------------------------------------------------------------------
