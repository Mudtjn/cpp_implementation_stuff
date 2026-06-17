#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <vector>

struct foo {
  foo() = default; // only special class member functions can be defaulted
  foo(foo const &) = delete;
};
void func(int) = delete;

// non-copyable class
class non_copyable_class {
public:
  non_copyable_class() = default;
  // default is helpful in sense
  // if we define move constructor, then
  // defaulting copy ctor and assignment generates them
  // even though compiler won't generate it automatically.
  non_copyable_class(non_copyable_class const &) = delete;
  non_copyable_class &operator=(non_copyable_class const &) = delete;
};

///////// ENSURING FIX TYPE OF PROMOTION AND PREVENTING OTHERS /////////////
template <typename T>
void run(T val) = delete; // =delete must be first declared
void run(long val) {}     // only long val being passed is allowed

// wrong delete order
// void to_be_deleted_function();
// void to_be_deleted_function() = delete;

////////////////////////////////////////////////////////////////////////////

// expression returned by lambda function
std::function<int(int const)> fib_create() {
  std::function<int(int const)> f{[](int const n) {
    std::function<int(int const)> lfib = [&lfib](int n) {
      return n <= 2 ? 1 : lfib(n - 1) + lfib(n - 2);
    };
    return lfib(n);
  }};
  return f;
};

struct fibonacci {
  std::function<int(int const)> lfib{
      [this](int const n) { return n <= 2 ? 1 : lfib(n - 1) + lfib(n - 2); }};
};

int main() {

  ///////// LAMBDA EXPRESSIONS //////////////////////////////////////////////
  std::vector<int> numbers{0, 2, -3, 5, -1};
  // eg to count positive nums
  auto positives{std::count_if(begin(numbers), end(numbers),
                               [](int const n) { return n > 0; })};
  // method 2 of defining lambda
  auto ispositive{[](int const n) { return n > 0; }};
  auto positives_way2{std::count_if(begin(numbers), end(numbers), ispositive)};
  // method 3 -> generic lambda
  auto positives_way3{std::count_if(begin(numbers), end(numbers),
                                    [](auto const n) { return n > 0; })};

  /// COMPILER DEFINITION OF LAMBDAS
  // struct __lambda_name__ // unnamed function
  // {
  //     bool operator()(int const n) {return n > 0; }
  // };
  // -> also generates move, copy ctor , move, copy assignment, default
  // destructor, deleted assignment operator

  ///////////////////// WAYS OF DEFINING LAMBDA /////////////////////////////
  // [](){} -> does not capture anything
  // [&](){} -> captures everything by reference
  // [=](){} -> captures everything by copy deprecated after c++20
  // [&x](){} -> captures only x by reference
  // [x](){} -> captures only x by copy
  // [&x...](){} -> captures pack extension by copy
  // [x...](){} -> captures pack extension by copy
  // [&, x](){} -> captures everything excepty x by reference , x by copy
  // [=, &x](){} -> captures everythinh except x by copy
  // pointer is always captured by copy
  // [x=expr](){} -> x is a data member of the lambda's closure initialized from
  // expression expr eg.
  auto y{10};
  auto b{[x = y + 1]() { /* x is y+1 */ }};
  auto c{[x = std::move(y)]() { /* x is moved-from y */ }};
  auto d{[ptr = std::make_unique<int>(42)]() { /* ptr is a unique ptr */ }};

  ///////////////////// WRITING GENERIC LAMBDAS ///////////////////////////////
  {
    using namespace std::string_literals;
    std::vector<std::string> string_literals{"Hello"s, "world"s};
    auto sum{[](auto const s, auto const n) { return s + n; }};

    auto sum1{std::accumulate(std::begin(numbers), std::end(numbers), 0, sum)};
    auto sum2{std::accumulate(std::begin(string_literals),
                              std::end(string_literals), ""s, sum)};

    std::cout << sum1 << std::endl << sum2 << std::endl;
  }

  ///////////////////////// TEMPLATING IN LAMBDAS ////////////////////
  {
    auto tl{[]<typename T>(std::vector<T> const &vec) {
      for (T x : vec) {
        std::cout << x << ' ';
      }
      std::cout << std::endl;
    }};
    tl(numbers);
  }

  //////////////////////////// RECURSIVE LAMDBA ///////////////////////
  {

    //   [&lfib] (int const n) {
    //     return n <= 2 ? 1 : lfib(n-1) + lfib(n-2);
    //   };
    // };
    // std::cout << lfib(10) << std::endl;
    {
      auto lfib{fib_create()};
    }
    auto lfib{[](auto f, int const n) {
      return n <= 2 ? 1 : f(f, n - 1) + f(f, n - 2);
    }};
    auto lfib2{[](this const auto &self, int n) {
      return n <= 2 ? 1 : self(n - 1) + self(n - 2);
    }};
  }

  ///////////////////////////////////////////////////////////////////////////
  return 0;
}
