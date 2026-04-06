#include <cstddef>
#include <iostream>
#include <iterator>
#include <stdexcept>

template <typename T> class Vector {

public:
  // typedefinitions
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = T &;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using rvalue = T &&;
  using const_rvalue = const T &&;

  using iterator = T *;
  using const_iterator = const T *;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  //////////// CONSTRUCTORS //////////////
  Vector();
  Vector(size_type size);
  Vector(size_type size, const_reference value);
  Vector(std::initializer_list<T> v);
  Vector(Vector &other_vec);           // copy constructor
  Vector(Vector &&other_vec) noexcept; // move constructor

  ////////////// ASSIGNMENT ////////////////////
  Vector &operator=(Vector &other);  // copy assignment
  Vector &operator=(Vector &&other); // move assignment

  ////////////// DESTRUCTORS ////////////////////
  ~Vector();

  ////////////// MODIFIERS //////////////////////
  void push_back(const_reference x);
  void push_back(rvalue x);
  void pop_back();
  void clear();
  // TODO: to implement insert, insert_range
  // inserts an element at a pos
  iterator insert(const_iterator pos, const_reference value);
  iterator insert_range(const_iterator pos, rvalue value);
  // TODO: implemnent emplace, emplace_back, append_range
  // iterator emplace(const_iterator pos, Args&& args);
  // erase
  // RETURNS: iterator following last removed elements
  // 1. If pos refers to last elements, then end() is returned
  // 2. If last==end(), then updated end() iterator is returned.
  // 2.1 if [first, last] is empty range -> then last is returned.
  // erases element at specific position
  iterator erase(iterator pos);
  iterator erase(const_iterator pos);
  // erases range of elements [first, last]
  iterator erase(iterator first, iterator last);
  iterator erase(const_iterator first, const_iterator last);
  void swap(Vector &other) noexcept;
  /**
   * @param count -> resizes the container to contain count elements
   * count == current_size : do nothing
   * count < current_size : container reduced to count elements
   * count > current_size :
   * 1. Additional copies of T() created.
   * 2. Additional copies of value are appended
   */
  void resize(size_type count);
  void resize(size_type count, const_reference value);

  /////////////// ELEMENTS ACCESS //////////////
  // to be allowed to be replaced
  reference at(std::size_t indx);
  const_reference at(std::size_t indx) const;
  reference operator[](std::size_t indx) const;
  reference front() const;
  reference back() const;
  pointer data() const;

  //////////////// ITERATORS //////////////////
  iterator begin();
  const_iterator cbegin() const;
  iterator end();
  const_iterator cend();
  reverse_iterator rbegin();
  const_reverse_iterator rbegin() const;
  const_reverse_iterator crebgin() const noexcept;
  reverse_iterator rend();
  reverse_iterator rend() const;
  const_reverse_iterator crend() const noexcept;

  //////////////// CAPACITY ///////////////////
  size_type capacity() const;
  size_type size() const;
  bool empty() const;
  void reserve(size_type new_cap);
  void shrink_to_fit();

private:
  pointer arr;
  size_type sz;
  size_type cap;

  void expandArray();
  void reserve_internal(size_type new_cap);
  void shrink_to_fit_internal();
};

template <typename T> void Vector<T>::expandArray() {
  if (cap == 0)
    cap = 1;
  cap *= 2;
  T *arr2{new T[cap]};
  std::copy(arr, arr + sz, arr2);
  delete[] arr;
  arr = arr2;
}

template <typename T> void Vector<T>::reserve_internal(std::size_t new_cap) {
  // create new storage of cap = new_cap
  T *arr2{new T[new_cap]};
  std::copy(arr, arr + sz, arr2);
  delete[] arr;
  arr = arr2;
}

template <typename T> void Vector<T>::shrink_to_fit_internal() {
  T *arr2{new T[sz]};
  std::copy(arr, arr + sz, arr2);
  cap = sz;
  delete[] arr;
  arr = arr2;
}

///////////////////////////////////////////////
//////////////// CONSTRUCTORS /////////////////
///////////////////////////////////////////////
template <typename T> Vector<T>::Vector() : arr(nullptr), sz(0), cap(0) {}
template <typename T> Vector<T>::Vector(size_type size) : sz(size), cap(size) {
  arr = new T[cap];
}
template <typename T>
Vector<T>::Vector(size_type size, const_reference value) : sz(size), cap(size) {
  arr = new T[cap];
  std::fill(arr, arr + sz, value);
}
template <typename T>
Vector<T>::Vector(std::initializer_list<T> v) : sz(v.size()), cap(v.size()) {
  arr = new T[cap];
  std::copy(v.begin(), v.end(), arr);
}
template <typename T>
Vector<T>::Vector(Vector &other) : sz{other.size()}, cap{other.capacity()} {
  if (cap == 0) {
    // delete not required since it is a constructor
    arr = nullptr;
  } else {
    auto *arr1 = new T[cap];
    for (auto i = 0; i < sz; i++) {
      arr1[i] = other[i];
    }
    // delete not required since it is a constructor
    arr = arr1;
  }
}
template <typename T>
Vector<T>::Vector(Vector &&other) noexcept
    : sz{other.size()}, cap{other.capacity()}, arr{other.arr} {
  // delete current pointers
  other.arr = nullptr;
  other.sz = 0;
  other.cap = 0;
}

///////////////////////////////////////////////
////////////// ASSIGNMENTS ////////////////////
///////////////////////////////////////////////
template <typename T> Vector<T> &Vector<T>::operator=(Vector &other) {

  sz = other.size();
  cap = other.capacity();
  if (cap == 0) {
    // delete constructor is required as this is assignment
    if (arr != nullptr)
      delete[] arr;
    arr = nullptr;
  } else {
    auto *arr1 = new T[cap];
    for (auto i = 0; i < sz; i++) {
      arr1[i] = other[i];
    }
    delete[] arr;
    arr = arr1;
  }

  // VERY IMP: FOR OPERATOR
  return *this;
}
template <typename T> Vector<T> &Vector<T>::operator=(Vector &&other) {
  if (this == &other) {
    return *this;
  }
  if (arr != nullptr) {
    delete[] arr;
  }
  arr = other.arr;
  other.arr = nullptr;
  sz = other.size();
  other.sz = 0;
  cap = other.capacity();
  other.cap = 0;

  return *this;
}

///////////////////////////////////////////////
////////////// DESTRUCTORS ////////////////////
///////////////////////////////////////////////
template <typename T> Vector<T>::~Vector() {
  delete[] arr;
  sz = cap = 0;
}

//////////////////////////////////////////////
/////////////// ELEMENTS ACCESS //////////////
//////////////////////////////////////////////
// to be allowed to be replaced
template <typename T> T &Vector<T>::at(size_type indx) {
  if (indx >= sz)
    return std::out_of_range("Out of index range");
  return arr[indx];
}
template <typename T>
typename Vector<T>::const_reference Vector<T>::at(size_type indx) const {
  if (indx >= sz)
    return std::out_of_range("Out of index range");
  return arr[indx];
}
template <typename T>
typename Vector<T>::reference Vector<T>::operator[](std::size_t indx) const {
  return at(indx);
}
template <typename T> typename Vector<T>::reference Vector<T>::front() const {
  return at(0);
}
template <typename T> typename Vector<T>::reference Vector<T>::back() const {
  return at(sz - 1);
}
template <typename T> typename Vector<T>::pointer Vector<T>::data() const {
  return arr;
}

/////////////////////////////////////////////
//////////////// CAPACITY ///////////////////
/////////////////////////////////////////////
template <typename T>
typename Vector<T>::size_type Vector<T>::capacity() const {
  return cap;
}
template <typename T> typename Vector<T>::size_type Vector<T>::size() const {
  return sz;
}
template <typename T> bool Vector<T>::empty() const { return size() == 0; }
template <typename T> void Vector<T>::reserve(std::size_t new_cap) {
  // if new_cap <= current_cap (do nothing)
  if (new_cap <= capacity()) {
    return;
  }
  // else create new storage and copy all elements
  reserve_internal(new_cap);
}
template <typename T> void Vector<T>::shrink_to_fit() {
  shrink_to_fit_internal();
}

////////////// MODIFIERS //////////////////////
template <typename T> void Vector<T>::push_back(const_reference x) {
  if (size() == capacity()) {
    expandArray();
  }
  arr[size()] = T{x};
}
template <typename T> void Vector<T>::push_back(rvalue x) {
  if (size() == capacity()) {
    expandArray();
  }
  arr[size()] = T{x};
}
template <typename T> void Vector<T>::pop_back() {
  if (size() == 0) {
    throw std::logic_error("Empty array cannot empty further\n");
  }
  sz--;
}
template <typename T> void Vector<T>::clear() { sz = 0; }
template <typename T> void Vector<T>::swap(Vector &other) noexcept {
  auto *arr_temp = arr;
  auto s1{sz}, cp1{cap};
  arr = other.arr;
  sz = other.size();
  cap = other.capacity();

  other.arr = arr_temp;
  other.sz = s1;
  other.cap = cp1;
}
template <typename T> void Vector<T>::resize(size_type count) {
  if (size() == count) {
    return;
  } else if (size() > count) {
    sz = count;
  } else {
    for (auto i{size()}; i < count; i++) {
      push_back(T{});
    }
  }
}
template <typename T>
void Vector<T>::resize(size_type count, const_reference value) {
  if (size() == count) {
    return;
  } else if (size() > count) {
    throw std::logic_error("current size of vector > count");
  } else {
    for (auto i{size()}; i < count; i++) {
      push_back(T{value});
    }
  }
}

///////////////////////////////////////////////////
///////////////// ITERATORS ///////////////////////
///////////////////////////////////////////////////

template <typename T> typename Vector<T>::iterator Vector<T>::begin() {
  return arr;
}
template <typename T>
typename Vector<T>::const_iterator Vector<T>::cbegin() const {
  return arr;
}
template <typename T> typename Vector<T>::iterator Vector<T>::end() {
  return arr + size();
}
template <typename T> typename Vector<T>::const_iterator Vector<T>::cend() {
  return arr + size();
}
template <typename T> typename Vector<T>::reverse_iterator Vector<T>::rbegin() {
  return std::reverse_iterator(end());
}
template <typename T>
typename Vector<T>::const_reverse_iterator Vector<T>::rbegin() const {
  return std::reverse_iterator(cend());
}
template <typename T>
typename Vector<T>::const_reverse_iterator Vector<T>::crebgin() const noexcept {
  return std::reverse_iterator(cend());
}
template <typename T> typename Vector<T>::reverse_iterator Vector<T>::rend() {
  return std::reverse_iterator(begin());
}
template <typename T>
typename Vector<T>::reverse_iterator Vector<T>::rend() const {
  return std::reverse_iterator(cbegin());
}
template <typename T>
typename Vector<T>::const_reverse_iterator Vector<T>::crend() const noexcept {
  return std::reverse_iterator(cbegin());
}

