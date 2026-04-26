#include "vector.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <initializer_list>
#include <string>
#include <vector>
#include <iterator>

// Existing tests
TEST(CustomVectorTest, CheckVectorSize) {
  Vector<int> v1(10);
  std::vector<int> v2(10);
  EXPECT_EQ(v1.size(), v2.size());
}

TEST(CustomVectorTest, CheckVectorInitializerList) {
  auto x = {1, 2, 3, 4, 5};
  Vector<int> v1{x};
  std::vector<int> v2{x};
  for (auto i = 0u; i < x.size(); i++) {
    EXPECT_EQ(v1[i], v2[i]);
  }
}

TEST(CustomVectorTest, CheckVectorSizeValueInitialization) {
  auto x{10u};
  std::string y{"Hello World"};
  Vector<std::string> v1(x, y);
  std::vector<std::string> v2(x, y);
  for (auto i = 0u; i < x; i++) {
    EXPECT_EQ(v1[i], v2[i]);
  }
}

// ---------- Constructors / assignment ----------

TEST(CustomVectorTest, DefaultConstructorCreatesEmptyVector) {
  Vector<int> v1;
  std::vector<int> v2;
  EXPECT_EQ(v1.size(), v2.size());
  EXPECT_EQ(v1.capacity(), v2.capacity());
  EXPECT_TRUE(v1.empty());
}

TEST(CustomVectorTest, SizeConstructorInitializesSize) {
  Vector<int> v1(5);
  std::vector<int> v2(5);
  EXPECT_EQ(v1.size(), v2.size());
  EXPECT_EQ(v1.empty(), v2.empty());
}

TEST(CustomVectorTest, CopyConstructorCopiesElements) {
  Vector<int> v1{1, 2, 3, 4};
  Vector<int> v2(v1);
  EXPECT_EQ(v1.size(), v2.size());
  for (std::size_t i = 0; i < v1.size(); ++i) {
    EXPECT_EQ(v1[i], v2[i]);
  }
}

TEST(CustomVectorTest, MoveConstructorTransfersOwnership) {
  Vector<int> v1{1, 2, 3};
  Vector<int> v2(std::move(v1));
  EXPECT_EQ(v2.size(), 3u);
  EXPECT_EQ(v2[0], 1);
  EXPECT_EQ(v2[1], 2);
  EXPECT_EQ(v2[2], 3);
  EXPECT_EQ(v1.size(), 0u);
  EXPECT_TRUE(v1.empty());
}

TEST(CustomVectorTest, CopyAssignmentCopiesElements) {
  Vector<int> v1{1, 2, 3};
  Vector<int> v2;
  v2 = v1;
  EXPECT_EQ(v1.size(), v2.size());
  for (std::size_t i = 0; i < v1.size(); ++i) {
    EXPECT_EQ(v1[i], v2[i]);
  }
}

TEST(CustomVectorTest, MoveAssignmentTransfersOwnership) {
  Vector<int> v1{10, 20, 30};
  Vector<int> v2;
  v2 = std::move(v1);
  EXPECT_EQ(v2.size(), 3u);
  EXPECT_EQ(v2[0], 10);
  EXPECT_EQ(v2[1], 20);
  EXPECT_EQ(v2[2], 30);
  EXPECT_EQ(v1.size(), 0u);
  EXPECT_TRUE(v1.empty());
}

// ---------- Element access ----------

TEST(CustomVectorTest, AtReturnsElement) {
  Vector<int> v1{1, 2, 3};
  std::vector<int> v2{1, 2, 3};
  EXPECT_EQ(v1.at(0), v2.at(0));
  EXPECT_EQ(v1.at(1), v2.at(1));
  EXPECT_EQ(v1.at(2), v2.at(2));
}

TEST(CustomVectorTest, AtThrowsOnOutOfRange) {
  Vector<int> v1{1, 2, 3};
  EXPECT_THROW(v1.at(3), std::out_of_range);
  EXPECT_THROW(v1.at(100), std::out_of_range);
}

TEST(CustomVectorTest, FrontBackMatchStdVector) {
  Vector<int> v1{5, 6, 7, 8};
  std::vector<int> v2{5, 6, 7, 8};
  EXPECT_EQ(v1.front(), v2.front());
  EXPECT_EQ(v1.back(), v2.back());
}

TEST(CustomVectorTest, DataPointerNotNullWhenNonEmpty) {
  Vector<int> v1{1, 2, 3};
  EXPECT_NE(v1.data(), nullptr);
}

// ---------- Modifiers ----------

TEST(CustomVectorTest, PushBackIncreasesSizeAndMatchesStdVector) {
  Vector<int> v1;
  std::vector<int> v2;
  for (int i = 0; i < 10; ++i) {
    v1.push_back(i);
    v2.push_back(i);
  }
  EXPECT_EQ(v1.size(), v2.size());
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(v1[i], v2[i]);
  }
}

TEST(CustomVectorTest, PopBackDecreasesSize) {
  Vector<int> v1{1, 2, 3, 4, 5};
  std::size_t old_size = v1.size();
  v1.pop_back();
  EXPECT_EQ(v1.size(), old_size - 1);
  EXPECT_EQ(v1.back(), 4);
}

TEST(CustomVectorTest, PopBackThrowsOnEmpty) {
  Vector<int> v1;
  EXPECT_THROW(v1.pop_back(), std::logic_error);
}

TEST(CustomVectorTest, ClearResetsSizeButKeepsCapacity) {
  Vector<int> v1{1, 2, 3, 4};
  auto old_cap = v1.capacity();
  v1.clear();
  EXPECT_EQ(v1.size(), 0u);
  EXPECT_EQ(v1.capacity(), old_cap);
  EXPECT_TRUE(v1.empty());
}

TEST(CustomVectorTest, ResizeSmallerReducesSize) {
  Vector<int> v1{1, 2, 3, 4, 5};
  v1.resize(3);
  EXPECT_EQ(v1.size(), 3u);
  EXPECT_EQ(v1[0], 1);
  EXPECT_EQ(v1[1], 2);
  EXPECT_EQ(v1[2], 3);
}

TEST(CustomVectorTest, ResizeLargerValueDefault) {
  Vector<int> v1{1, 2};
  v1.resize(5);
  EXPECT_EQ(v1.size(), 5u);
  EXPECT_EQ(v1[0], 1);
  EXPECT_EQ(v1[1], 2);
  // Just check that it doesn't crash and size is correct; default values depend
  // on T
}

TEST(CustomVectorTest, ResizeWithValueWhenShrinking) {
 Vector<int> v1{1, 2, 3, 4};
 std::vector<int> v2{1, 2, 3, 4}; 
 v1.resize(2, 10);
 v2.resize(2, 10);
 EXPECT_EQ(v1.size(), v2.size()); 
 for(auto i{0} ; i<v1.size() ; i++) {
  EXPECT_EQ(v1[i], v2[i]); 
 }
}

TEST(CustomVectorTest, ResizeWithValueGrowsAndFills) {
  Vector<int> v1{1, 2};
  v1.resize(5, 10);
  EXPECT_EQ(v1.size(), 5u);
  EXPECT_EQ(v1[0], 1);
  EXPECT_EQ(v1[1], 2);
  EXPECT_EQ(v1[2], 10);
  EXPECT_EQ(v1[3], 10);
  EXPECT_EQ(v1[4], 10);
}

TEST(CustomVectorTest, SwapExchangesContents) {
  Vector<int> v1{1, 2, 3};
  Vector<int> v2{10, 20};
  v1.swap(v2);
  EXPECT_EQ(v1.size(), 2u);
  EXPECT_EQ(v1[0], 10);
  EXPECT_EQ(v1[1], 20);
  EXPECT_EQ(v2.size(), 3u);
  EXPECT_EQ(v2[0], 1);
  EXPECT_EQ(v2[1], 2);
  EXPECT_EQ(v2[2], 3);
}

// ---------- Capacity / reserve / shrink_to_fit ----------

TEST(CustomVectorTest, ReserveIncreasesCapacityButNotSize) {
  Vector<int> v1{1, 2, 3};
  auto old_size = v1.size();
  auto old_cap = v1.capacity();
  v1.reserve(old_cap + 10);
  EXPECT_EQ(v1.size(), old_size);
  EXPECT_GE(v1.capacity(), old_cap + 10);
  EXPECT_EQ(v1[0], 1);
  EXPECT_EQ(v1[1], 2);
  EXPECT_EQ(v1[2], 3);
}

TEST(CustomVectorTest, ReserveNoChangeWhenNewCapSmaller) {
  Vector<int> v1{1, 2, 3};
  auto old_size = v1.size();
  auto old_cap = v1.capacity();
  v1.reserve(old_cap - 1); // should do nothing
  EXPECT_EQ(v1.size(), old_size);
  EXPECT_EQ(v1.capacity(), old_cap);
}

TEST(CustomVectorTest, ShrinkToFitReducesCapacityToSize) {
  Vector<int> v1;
  v1.reserve(100);
  v1.push_back(1);
  v1.push_back(2);
  v1.push_back(3);
  v1.shrink_to_fit();
  EXPECT_EQ(v1.capacity(), v1.size());
}

// ---------- Iterators ----------

TEST(CustomVectorTest, BeginEndIterateAllElements) {
  Vector<int> v1{1, 2, 3, 4};
  std::vector<int> v2{1, 2, 3, 4};
  auto it1 = v1.begin();
  auto it2 = v2.begin();
  for (; it1 != v1.end() && it2 != v2.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }
  EXPECT_EQ(it1, v1.end());
  EXPECT_EQ(it2, v2.end());
}

TEST(CustomVectorTest, ReverseIteratorsTraverseBackwards) {
  Vector<int> v1{1, 2, 3, 4};
  std::vector<int> v2{1, 2, 3, 4};
  auto it1 = v1.rbegin();
  auto it2 = v2.rbegin();
  for (; it1 != v1.rend() && it2 != v2.rend(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }
  EXPECT_EQ(it1, v1.rend());
  EXPECT_EQ(it2, v2.rend());
}

TEST(CustomVectorTest, ConstReverseIteratorsTraverseBackwards) {
  const Vector<int> v1{1, 2, 3, 4};
  const std::vector<int> v2{1, 2, 3, 4};
  auto it1 = v1.rbegin();
  auto it2 = v2.rbegin();
  for (; it1 != v1.crend() && it2 != v2.crend(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }
}

// --------------- Insert operation -------------
TEST(CustomVectorTest, InsertValueAtStart) {
  Vector<int> v1 {1, 2, 3, 4}; 
  std::vector<int> v2 {1, 2, 3,4}; 
  auto it1 = v1.insert(v1.begin(), 100); 
  auto it2 = v2.insert(v2.begin(), 100); 
  for(auto i{0} ; i<5 ; i++) {
    EXPECT_EQ(v1[i], v2[i]); 
  }
  EXPECT_EQ((it1 - v1.begin()), (it2 - v2.begin())); 
}

TEST(CustomVectorTest, InsertValueAtEnd) {
  Vector<int> v1 {1, 2, 3, 4}; 
  std::vector<int> v2 {1, 2, 3, 4}; 
  auto it1 = v1.insert(v1.end(), 100); 
  auto it2 = v2.insert(v2.end(), 100);
  for(auto i{0} ; i<5 ; i++) {
    EXPECT_EQ(v1[i], v2[i]); 
  }
  EXPECT_EQ((it1 - v1.begin()), (it2 - v2.begin())); 
}

TEST(CustomVectorTest, InsertValueInMiddle) {
  Vector<int> v1 {1, 2, 3, 4}; 
  std::vector<int> v2 {1, 2, 3, 4}; 
  auto it1 = v1.insert(v1.begin() + 2, 100); 
  auto it2 = v2.insert(v2.begin() + 2, 100);
  for(auto i{0} ; i<5 ; i++) {
    EXPECT_EQ(v1[i], v2[i]); 
  }
  EXPECT_EQ((it1 - v1.begin()), (it2 - v2.begin())); 
}

TEST(CustomVectorTest, InsertValuesAtStart) {
  Vector<int> v1 {1, 2, 3, 4}; 
  std::vector<int> v2 {1, 2, 3,4}; 
  auto it1 = v1.insert(v1.begin(), 100, 10); 
  auto it2 = v2.insert(v2.begin(), 100, 10); 
  for(auto i{0} ; i<104 ; i++) {
    EXPECT_EQ(v1[i], v2[i]); 
  }
  EXPECT_EQ((it1 - v1.begin()), (it2 - v2.begin())); 
}

TEST(CustomVectorTest, InsertValuesAtEnd) {
  Vector<int> v1 {1, 2, 3, 4}; 
  std::vector<int> v2 {1, 2, 3, 4}; 
  auto it1 = v1.insert(v1.end(), 100, 10); 
  auto it2 = v2.insert(v2.end(), 100, 10);
  for(auto i{0} ; i<104 ; i++) {
    EXPECT_EQ(v1[i], v2[i]); 
  }
  EXPECT_EQ((it1 - v1.begin()), (it2 - v2.begin())); 
}

TEST(CustomVectorTest, InsertValuesInMiddle) {
  Vector<int> v1 {1, 2, 3, 4}; 
  std::vector<int> v2 {1, 2, 3, 4}; 
  auto it1 = v1.insert(v1.begin() + 2, 100, 10); 
  auto it2 = v2.insert(v2.begin() + 2, 100, 10);
  for(auto i{0} ; i<104 ; i++) {
    EXPECT_EQ(v1[i], v2[i]); 
  }
  EXPECT_EQ((it1 - v1.begin()), (it2 - v2.begin())); 
}

TEST(CustomVectorTest, InsertRange) {
  std::vector<std::string> vec_to_insert {"10", "10", "20"}; 
  Vector<std::string> v1 {"1", "2", "3", "4"}; 
  std::vector<std::string> v {"1", "2", "3", "4"}; 

  auto it1 = v1.insert(v1.begin() + 1, vec_to_insert.begin()+1, vec_to_insert.end()); 
  auto it2 = v.insert(v.begin() + 1, vec_to_insert.begin()+1, vec_to_insert.end()); 
  
  EXPECT_EQ(v1.size(), v.size());
  // for(auto x: v1) {
  //   std::cout << x << ' '; 
  // } 
  // std::cout << "\n"; 
  // for(auto x: v) {
  //   std::cout << x << ' ';  
  // }
  // std::cout << "\n"; 
  for(auto i{0} ; i<v1.size() ; i++) {
    EXPECT_EQ(v1[i], v[i]); 
  }
  EXPECT_EQ((it1 - v1.begin()), (it2 - v.begin())); 
}

TEST(CustomVectorTest, InsertRangeReverse) {
  std::vector<std::string> vec_to_insert {"10", "10", "20"}; 
  Vector<std::string> v1 {"1", "2", "3", "4"}; 
  std::vector<std::string> v {"1", "2", "3", "4"}; 

  auto it1 = v1.insert(v1.begin() + 1, vec_to_insert.rbegin()+1, vec_to_insert.rend()); 
  auto it2 = v.insert(v.begin() + 1, vec_to_insert.rbegin()+1, vec_to_insert.rend()); 
  
  EXPECT_EQ(v1.size(), v.size()); 
  for(auto i{0} ; i<v1.size() ; i++) {
    EXPECT_EQ(v1[i], v[i]); 
  }
  EXPECT_EQ((it1 - v1.begin()), (it2 - v.begin())); 
}

TEST(CustomVectorTest, InsertInitializerList) {
  std::initializer_list<std::string> list_to_insert {"A", "B"}; 
  Vector<std::string> v1 {"1", "2", "3", "4"}; 
  std::vector<std::string> v {"1", "2", "3", "4"}; 

  auto it1 = v1.insert(v1.begin() + 1, list_to_insert.begin()+1, list_to_insert.end()); 
  auto it2 = v.insert(v.begin() + 1, list_to_insert.begin()+1, list_to_insert.end()); 
  
  EXPECT_EQ(v1.size(), v.size()); 
  for(auto i{0} ; i<v1.size() ; i++) {
    EXPECT_EQ(v1[i], v[i]); 
  }
  EXPECT_EQ((it1 - v1.begin()), (it2 - v.begin()));
}

TEST(CustomVectorTest, SortVectorUsingStdSort) {
  Vector<int> v1 {4, 3, 2, 1}; 
  std::vector<int> v2 {4, 3, 2, 1};
  std::sort(v1.begin(), v1.end()); 
  std::sort(v2.begin(), v2.end());

  EXPECT_EQ(v1.size(), v2.size()); 
  for(auto i{0} ; i<v1.size() ; i++) {
    EXPECT_EQ(v1[i], v2[i]); 
  }
}

TEST(CustomVectorTest, ReverseVectorUsingStdReverse) {
  Vector<int> v1 {4, 3, 2, 1}; 
  std::vector<int> v2 {4, 3, 2, 1};
  std::reverse(v1.begin(), v1.end()); 
  std::reverse(v2.begin(), v2.end());

  EXPECT_EQ(v1.size(), v2.size()); 
  for(auto i{0} ; i<v1.size() ; i++) {
    EXPECT_EQ(v1[i], v2[i]); 
  }
}

TEST(CustomVectorTest, InsertRangeUsingVector) {
  Vector<int> v1 {1, 2, 3, 4};
  std::vector<int> v2 {1, 2, 3, 4};
  const auto rg = std::vector<int> {-1, -2, -3};

  auto pos_v1 {std::next(v1.begin(), 2)};
  auto pos_v2 {std::next(v2.begin(), 2)};

  EXPECT_EQ(*pos_v1, 3);
  auto it1 {v1.insert_range(pos_v1, rg)};
  auto it2 {v2.insert(pos_v2, rg.begin(), rg.end())};  // insert_range needs libstdc++14+; not available on this system
  EXPECT_EQ(v1.size(), v2.size()); 
  for (std::size_t i = 0; i < v2.size(); i++) {
    EXPECT_EQ(v1[i], v2[i]);
  }
  EXPECT_EQ(it1 - v1.begin(), it2 - v2.begin());
}

TEST(CustomVectorTest, AppendRangeUsingVector) {
  Vector<int> v1 {1, 2, 3, 4};
  std::vector<int> v2 {1, 2, 3, 4};
  const auto rg = std::vector<int> {-1, -2, -3};

  v1.append_range(rg);
  v2.insert(v2.end(), rg.begin(), rg.end());  // append_range = C++23; not in libstdc++13
  EXPECT_EQ(v1.size(), v2.size());
  for (std::size_t i = 0; i < v2.size(); i++) {
    EXPECT_EQ(v1[i], v2[i]);
  }
}

// main unchanged
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
