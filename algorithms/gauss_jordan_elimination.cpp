#include <cstring>
#include <iostream>
#include <unordered_map>

float matrix[100][100];

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

int main() {
  std::memset(matrix, 0, sizeof(matrix));
  auto n{0}, m{0};
  std::cin >> m >> n; // m x n matrix input

  std::unordered_map<int, float *> mp;
  for (auto i{0}; i < m; i++) {
    for (auto j{0}; j < n; j++) {
      std::cin >> matrix[i][j];
    }
    mp[i] = matrix[i];
  }

  for (auto i{0}; i < std::min(m, n); i++) {
    mp[i][i + n] = 1;
  }

  auto print_matrix{[&]() {
    for (auto i{0}; i < m; i++) {
      for (auto j{0}; j < 2 * n; j++) {
        std::cout << mp[i][j] << ' ';
        if (j == n - 1)
          std::cout << " || ";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;
  }};
  // before processing result
  print_matrix();
  gauss_jordan_elimination(mp, m, n);
  // after processing result
  print_matrix();

  return 0;
}
