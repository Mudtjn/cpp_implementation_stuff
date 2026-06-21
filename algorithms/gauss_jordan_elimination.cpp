#include <algorithm>
#include <chrono>
#include <cstring>
#include <execution>
#include <iostream>
#include <omp.h>
#include <random>
#include <ratio>
#include <unordered_map>


float matrix[1000][1000];
float matrix_for_seq[2000][2000];
float matrix_for_parallel[2000][2000];

void gauss_jordan_elimination(std::unordered_map<int, float *> h, int m,
                              int n) {
  // reduce to row echelon form
  // find pivot and make all entries below it zero
  for (auto i{0}; i < m;) {
    if (h[i][i] == 0) {
      // find first non_zero ixi entry and replace row with it
      for (auto j{i + 1}; j < m; j++) {
        if (h[j][i]) {
          std::swap(h[i], h[j]);
          break;
        }
      }
    } else {
      for (auto j{i + 1}; j < m; j++) {
        auto multiplier{h[j][i]};
        for (auto k{i}; k < 2 * n; k++) {
          // multiply row by h[i][i]
          h[j][k] *= h[i][i];
          h[j][k] -= h[i][k] * multiplier;
        }
      }
      i++;
    }
  }

  // ref to rref
  // bottom's up
  for (auto i{std::min(m, n) - 1}; i >= 0; i--) {
    if (h[i][i] == 0) {
      // already done pivot
      continue;
    } else {
      // divide entire row
      auto divisor{h[i][i]};
      for (auto k{i}; k < 2 * n; k++) {
        h[i][k] = h[i][k] / (divisor * 1.0);
      }

      // subtract that from each row
      for (auto j{i - 1}; j >= 0; j--) {
        auto multiplier{h[j][i]};
        for (auto k{0}; k < 2 * n; k++) {
          h[j][k] -= h[i][k] * multiplier;
        }
      }
    }
  }
}

void gauss_jordan_elimination_parallel(std::unordered_map<int, float *> h,
                                       int m, int n) {
  // reduce to row echelon form
  // find pivot and make all entries below it zero
  for (auto i{0}; i < m;) {
    if (h[i][i] == 0) {
      // find first non_zero ixi entry and replace row with it
      for (auto j{i + 1}; j < m; j++) {
        if (h[j][i]) {
          std::swap(h[i], h[j]);
          break;
        }
      }
    } else {
      for (auto j{i + 1}; j < m; j++) {
        auto multiplier{h[j][i]};
        auto multiplier_2{h[i][i]};
        ////////////////////////// IMPORTANT /////////////////////////////////
        // This kind of nested parallelism does not work and can cause deadlocks
        // iterator moves to next row and starts operation
        // causing deadlocks
        std::for_each(std::execution::par, h[j] + i, h[j] + 2 * n,
                      [i, multiplier, multiplier_2, h_row_j = h[j],
                       h_row_i = h[i]](float &x) {
                        auto k{&x - h_row_j + i};
                        x = x * multiplier_2 - h_row_i[k] * multiplier;
                      });
      }
      i++;
    }
  }
  // ref to rref
  // bottom's up
  for (auto i{std::min(m, n) - 1}; i >= 0; i--) {
    if (h[i][i] == 0) {
      // already done pivot
      continue;
    } else {
      // divide entire row
      auto divisor{h[i][i]};
      std::transform(std::execution::par, h[i] + i, h[i] + 2 * n, h[i] + i,
                     [divisor](float x) { return x / (divisor * 1.0); });
      // subtract that from each row
      for (auto j{i - 1}; j >= 0; j--) {
        auto multiplier{h[j][i]};
        std::for_each(std::execution::par, h[j] + 0, h[j] + 2 * n,
                      [i, multiplier, h_row_j = h[j], h_row_i = h[i]](float x) {
                        auto k{&x - h_row_j};
                        x -= h_row_i[k] * multiplier;
                      });
      }
    }
  }
}

void gauss_jordan_elimination_parallel_corrected(
    std::unordered_map<int, float *> h, int m, int n) {
  // reduce to row echelon form
  // find pivot and make all entries below it zero
  for (auto i{0}; i < m;) {
    if (h[i][i] == 0) {
      // find first non_zero ixi entry and replace row with it
      for (auto j{i + 1}; j < m; j++) {
        if (h[j][i]) {
          std::swap(h[i], h[j]);
          break;
        }
      }
    } else {
      // using openmp here
      auto pivot{h[i][i]};
#pragma omp parallel for
      for (auto j{i + 1}; j < m; j++) {
        auto multiplier{h[j][i]};
#pragma omp parallel for
        for (auto k{i}; k < 2 * n; k++) {
          h[j][k] = h[j][k] * pivot - h[i][k] * multiplier;
        }
      }
      i++;
    }
  }
  // ref to rref
  // bottom's up
  for (auto i{std::min(m, n) - 1}; i >= 0; i--) {
    if (h[i][i] == 0) {
      // already done pivot
      continue;
    } else {
      // divide entire row
      auto divisor{h[i][i]};
#pragma omp parallel for
      for (auto k{i}; k < 2 * n; k++) {
        h[i][k] = h[i][k] / (divisor * 1.0);
      }

// subtract that from each row
#pragma omp parallel for
      for (auto j{i - 1}; j >= 0; j--) {
        auto multiplier{h[j][i]};
#pragma omp parallel for
        for (auto k{0}; k < 2 * n; k++) {
          h[j][k] -= h[i][k] * multiplier;
        }
      }
    }
  }
}

void fill_invertible_matrix(int m, int n, unsigned seed,
                            int num_transforms = -1) {
  // only makes sense for square (m == n) if you want true invertibility;
  // for m != n this still gives you a well-conditioned, full-rank-ish matrix
  std::memset(matrix, 0, sizeof(matrix));

  // step 1: identity
  for (int i = 0; i < m; i++)
    matrix[i][i] = 1.0f;

  std::mt19937 rng(seed);
  std::uniform_int_distribution<int> row_dist(0, m - 1);
  std::uniform_real_distribution<float> mult_dist(-5.0f, 5.0f);
  std::uniform_real_distribution<float> scale_dist(0.5f,
                                                   5.0f); // kept away from 0
  std::uniform_int_distribution<int> op_dist(
      0, 2); // 0=swap, 1=scale, 2=add-multiple

  int transforms =
      (num_transforms > 0) ? num_transforms : m * 4; // a handful per row

  for (int t = 0; t < transforms; t++) {
    int op = op_dist(rng);
    int r1 = row_dist(rng);

    if (op == 0) {
      // swap two distinct rows
      int r2 = row_dist(rng);
      if (r1 != r2)
        for (int k = 0; k < n; k++)
          std::swap(matrix[r1][k], matrix[r2][k]);
    } else if (op == 1) {
      // scale row by a nonzero factor
      float s = scale_dist(rng);
      for (int k = 0; k < n; k++)
        matrix[r1][k] *= s;
    } else {
      // add a multiple of one row to a different row
      int r2 = row_dist(rng);
      if (r1 != r2) {
        float mult = mult_dist(rng);
        for (int k = 0; k < n; k++)
          matrix[r1][k] += mult * matrix[r2][k];
      }
    }
  }

  // set up the augmented identity block your algorithm expects
  for (int i = 0; i < std::min(m, n); i++)
    matrix[i][i + n] = 1.0f;
}

void benchmark_setup(int n) {
  // populate matrix using rngs
  std::cout << "SETTING UP SOURCE MATRIX" << std::endl;
  for (auto i{0}; i < n; i++) {
    matrix[i][i] = 1;
  }
  fill_invertible_matrix(n, n, 234322);
}

void copy_from_source(float mat[][2000], int n,
                      std::unordered_map<int, float *> &mp) {
  for (auto i{0}; i < n; i++) {
    mp[i] = mat[i];
    std::memcpy(mat[i], matrix[i], n);
    mat[i][i + n] = 1;
  }
}

template <typename Func>
double time_run(Func f, std::unordered_map<int, float *> &h, int n) {
  auto start{std::chrono::high_resolution_clock::now()};
  f(h, n, n);
  auto end{std::chrono::high_resolution_clock::now()};
  return std::chrono::duration<double, std::milli>(end - start).count();
}

bool check_equal(float mat1[][2000], float mat2[][2000], int n) {
  for (auto i{0}; i < n; i++) {
    for (auto j{0}; j < n; j++) {
      if (mat1[i][j] != mat2[i][j])
        return false;
    }
  }
  return true;
}

void benchmark_run(int n, int turns) {
  // setup benchmark
  benchmark_setup(n);
  std::cout << "STARTING_BENCHMARK_RUNS" << std::endl;
  double seq_run{0}, parallel_runs{0};
  for (auto i{0}; i < turns; i++) {
    // copy from source matrix
    std::unordered_map<int, float *> mp1;
    copy_from_source(matrix_for_seq, n, mp1);
    auto seq_run_time{time_run(gauss_jordan_elimination, mp1, n)};
    std::cout << "SEQUENTIAL RUN TIME FOR RUN " << (i + 1) << " : "
              << seq_run_time << "ms\n";
    seq_run += seq_run_time;

    std::unordered_map<int, float *> mp2;
    copy_from_source(matrix_for_parallel, n, mp2);
    auto parallel_run_time{
        time_run(gauss_jordan_elimination_parallel_corrected, mp2, n)};
    std::cout << "PARALLEL RUN TIME FOR RUN " << (i + 1) << " : "
              << parallel_run_time << "ms\n";
    parallel_runs += parallel_run_time;

    check_equal(matrix_for_seq, matrix_for_parallel, n);
  }

  auto seq_avg{seq_run / turns}, par_avg{parallel_runs / turns};

  std::cout << "\nSEQUENTIAL RUNS AVERAGE: " << seq_avg << "ms" << std::endl;
  std::cout << "PARALLEL RUNS AVERAGE: " << par_avg << "ms" << std::endl;

  std::cout << "SPEEDUP: " << seq_avg / par_avg << std::endl;
}

int main() {
  std::memset(matrix, 0, sizeof(matrix));
  auto n{500};
  
  benchmark_run(n, 10);

  return 0;
}
