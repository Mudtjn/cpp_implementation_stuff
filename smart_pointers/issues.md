Nice work — this is a good start and shows you understand the main pieces a UniquePtr needs (pointer + deleter, move-only, release/reset/get, operator*, ->, bool, destructor). Below I list concrete bugs, missing features, and behavioral/semantic problems in your implementation, grouped by priority. I also include short suggestions or code sketches for fixes where helpful.

High priority (will break code / UB / compile errors)
- Destructor: calling deleter on nullptr is allowed, so the explicit if(ptr!=nullptr) check is unnecessary but harmless. However, the class lacks a user-provided move that nulls the source pointer; the destructor of a moved-from UniquePtr will still call deleter on the original pointer unless you clear the source. Your move constructors/assignments must set the source.ptr to nullptr.
  - Fix: in any move ctor/assign, set the source.ptr = nullptr afterwards.

- Move constructor template: 
  - You use u.ptr and u.get_deleter() but u is an lvalue in this function signature and you access its private member ptr (u.ptr) — since u is a different instantiation (UniquePtr<T,E>&& u), accessing u.ptr is allowed only if other instantiation friended or same template? In a templated member of the same class template you can access private members of different instantiations only in C++11+? Actually, UniquePtr<T,Deleter>::UniquePtr(UniquePtr<T,E>&& u) is a member of the same class template but different specialization; C++ allows access to private members of other specializations of the same template, so that's OK. Still, you must std::move the deleter correctly.
  - Major bug: you call std::forward<E>(u.get_deleter()). u.get_deleter() returns Deleter& (lvalue reference to the other type's deleter) — forwarding with E is wrong. You need to move the deleter value (via std::move(u.get_deleter())) if E is moveable; also you must conditionally handle when deleter type is not convertible/movable to this Deleter.
  - Also, you must null the source's pointer after taking ownership: u.ptr = nullptr.

- operator=(UniquePtr&&) implementation is broken (semantic and compile errors):
  - Signature: UniquePtr& operator=(UniquePtr&&); you implement UniquePtr<T,Deleter>& UniquePtr<T,Deleter>::operator=(UniquePtr&& ptr1) { if(ptr == ptr1) { return ptr1; } ptr = ptr1; } This is invalid for many reasons:
    - ptr is a pointer member; comparing ptr == ptr1 compares pointer to object vs object — nonsense. You intended to compare addresses or self-assignment but should compare this == &ptr1.
    - You return ptr1 (a UniquePtr) from a function returning UniquePtr&.
    - You assign ptr = ptr1 (pointer = object) — wrong.
    - You never delete the previously owned object before taking ownership, leaking memory.
    - You never move the deleter.
    - You never null out the source.
  - Correct pattern:
    - If (this != &other) { reset(); ptr = other.ptr; deleter = std::forward<Deleter>(other.deleter) (or std::move); other.ptr = nullptr; } return *this;
    - Or implement via swap: UniquePtr tmp(std::move(other)); swap(tmp); return *this;

- operator=(std::nullptr_t) implementation broken:
  - You wrote get_deleter()(ptr); That calls the deleter on ptr, but you do not set ptr to nullptr afterward and you do not return *this (signature returns UniquePtr&). Also calling deleter on nullptr is allowed but you should first delete current pointer and then set ptr = nullptr.
  - Fix: reset(nullptr); return *this;

- reset overloads / declarations weird and definitions missing:
  - You declare three reset overloads:
    - void reset(pointer ptr = pointer()) noexcept;
    - template<class U> void reset(U ptr) noexcept;
    - void reset(std::nullptr_t = nullptr) noexcept;
  - Then you don't define them (aside from operator=(nullptr) mistakenly using deleter). You need a single reset(pointer p = pointer()) that deletes the owned pointer (if any), sets ptr = p. Template reset(U) is unusual; unique_ptr has reset(pointer) only. Remove confusing overloads.
  - Must be careful with array specialization: unique_ptr<T[]> has different interface. For single-object unique_ptr, reset should accept pointer of type pointer and be noexcept when Deleter is noexcept on pointer.

- operator* return type is wrong for reference:
  - You used typename std::add_lvalue_reference<T>::type operator*() const noexcept(...). For T being non-reference, std::add_lvalue_reference<T>::type is T& (fine). But this will fail when T is void or array types. Also you marked operator* const but returning reference to non-mutable when T is non-const — standard unique_ptr::operator* returns T& (for non-const UniquePtr) and const T& for const UniquePtr. Your implementation is single const-qualified method returning T& even on const UniquePtr; that might be okay but be mindful of const-correctness.
  - Simpler: T& operator*() const { return *ptr; }

- operator[] returns T& but body returns ptr (pointer), not element: T& UniquePtr::operator[](size_t i) const { return ptr; } — this is wrong; should be return ptr[i]; Also operator[] should only exist for array specialization UniquePtr<T[],Deleter>. For single-object UniquePtr it should be omitted.

- UniquePtr(UniquePtr&&) duplicate declarations:
  - You declared a non-template UniquePtr(UniquePtr&& other_ptr); and also earlier template <class E> UniquePtr(UniquePtr<T,E>&& u) noexcept; This duplicates and the non-template version is defined nowhere. Remove the non-template variant or define it consistently (it can be a specialization of the template with E=Deleter). Prefer template constructor plus defaulted move.

- Deleted/const-correctness confusion:
  - You declare UniquePtr(UniquePtr&) = delete and UniquePtr(const UniquePtr&) = delete which is redundant; one deleted copy ctor would suffice. You declared UniquePtr(UniquePtr&& other_ptr); (non-noexcept) and also a template move — duplicate.
  - You declared UniquePtr(const UniquePtr&& other_ptr) = delete and operator=(const UniquePtr&&) = delete — those const rvalue ref types are odd and unnecessary. Remove them.

- get_deleter() returns non-const reference in non-const overload only; you provided const overload — ok. But your template move ctor’s use of get_deleter() must consider return type.

Medium priority (missing features, safety, conversions)
- Converting move constructor semantics:
  - The template converting move ctor should require that E is convertible to Deleter (i.e. std::is_convertible_v<E&, Deleter>) and pointer types are compatible. You need SFINAE constraints to enable/disable the template constructor correctly.
  - You should accept UniquePtr<U, E>&& where U* is convertible to T* (e.g., derived->base) and E convertible to Deleter.

- noexcept specifications:
  - Constructors/assignments should be noexcept when appropriate (e.g., move ctor noexcept if Deleter is nothrow move-constructible). Consider conditional noexcept: noexcept(std::is_nothrow_move_constructible_v<Deleter>).
  - reset and release may be noexcept depending on Deleter.

- get_deleter const-correctness and noexcept:
  - Provide noexcept on get_deleter when possible.

- release should return pointer and set ptr to nullptr. You declared pointer release() noexcept; but did not define it.

- swap should swap both ptr and deleter (use std::swap on deleter). Provide noexcept conditional on swap noexceptness.

- Missing relational comparisons and get() const noexcept done OK.

- Missing equality to nullptr operator=(nullptr) should be defined to delete owned object and set to null and return *this.

Low priority (style, efficiency)
- Use member initializer lists consistently and default move operations where sensible.
- Consider implementing assignment via copy-and-swap to be exception-safe.
- Consider providing explicit bool operator and marking it explicit (you did explicit operator bool()) — good.

Other specific coding issues/compilation errors
- Many member functions are declared but not defined (reset, release, swap, converting move ctor needs to null source, operator=(UniquePtr&&) broken, operator=(nullptr) missing return). Trying to compile will produce many link errors and incorrect behavior.
- The operator* signature uses noexcept(noexcept(*std::declval<pointer>())) — ok but overcomplicated.
- Using std::forward<E>(u.get_deleter()) is wrong: std::forward expects an expression of type E&&. You have an lvalue reference returned from get_deleter(); correct approach is std::move(u.get_deleter()) if you intend to move the deleter.

Suggested minimal correct implementation outline
- Keep simple: support single-object UniquePtr<T, Deleter = default_delete<T>>.
- Implement:
  - constructors: default, nullptr, explicit(pointer p), move ctor (UniquePtr(UniquePtr&&) noexcept(...)), template converting move ctor (UniquePtr(UniquePtr<U, E>&&) with SFINAE to ensure convertible)
  - destructor: call deleter(ptr);
  - release(): pointer p = ptr; ptr = nullptr; return p;
  - reset(pointer p = nullptr): if (ptr!=p) { deleter(ptr); ptr = p; }
  - swap(UniquePtr& o): std::swap(ptr, o.ptr); std::swap(deleter, o.deleter);
  - operator=(UniquePtr&& other): reset(); ptr = other.ptr; deleter = std::forward<Deleter>(other.deleter); other.ptr = nullptr; return *this;
  - operator=(nullptr_t): reset(); return *this;
  - get(), operator*, operator->, explicit operator bool().

Small example (sketch)
- Here’s a compact, correct sketch (not full SFINAE for converting constructor). Insert as a guideline — compile/test before use.

  template<class T, class D = std::default_delete<T>>
  class UniquePtr {
  public:
    using pointer = T*;
    // ctors
    constexpr UniquePtr() noexcept : ptr(nullptr), deleter() {}
    constexpr UniquePtr(std::nullptr_t) noexcept : UniquePtr() {}
    explicit UniquePtr(pointer p) noexcept : ptr(p), deleter() {}
    UniquePtr(UniquePtr&& o) noexcept(std::is_nothrow_move_constructible_v<D>)
      : ptr(o.ptr), deleter(std::move(o.deleter)) { o.ptr = nullptr; }
    // converting move ctor (U* -> T*)
    template<class U, class E>
    UniquePtr(UniquePtr<U,E>&& o) noexcept(/*condition*/)
      : ptr(o.ptr), deleter(std::move(o.get_deleter())) { o.ptr = nullptr; }
    ~UniquePtr() { if (ptr) deleter(ptr); }
    UniquePtr& operator=(UniquePtr&& o) noexcept(/*cond*/) {
      if (this != &o) {
        reset();
        ptr = o.ptr;
        deleter = std::move(o.deleter);
        o.ptr = nullptr;
      }
      return *this;
    }
    UniquePtr& operator=(std::nullptr_t) noexcept { reset(); return *this; }
    pointer release() noexcept { pointer p = ptr; ptr = nullptr; return p; }
    void reset(pointer p = nullptr) noexcept {
      pointer old = ptr; ptr = p;
      if (old) deleter(old);
    }
    void swap(UniquePtr& o) noexcept(noexcept(std::swap(ptr,o.ptr))) {
      std::swap(ptr, o.ptr); std::swap(deleter, o.deleter);
    }
    pointer get() const noexcept { return ptr; }
    D& get_deleter() noexcept { return deleter; }
    const D& get_deleter() const noexcept { return deleter; }
    explicit operator bool() const noexcept { return ptr != nullptr; }
    T& operator*() const { return *ptr; }
    pointer operator->() const noexcept { return ptr; }
  private:
    pointer ptr;
    D deleter;
  };

Testing notes and edge-cases
- Test polymorphic conversions: UniquePtr<Derived> -> UniquePtr<Base>.
- Test custom deleters that are stateful/non-copyable: ensure move operations work if deleter is moveable but not copyable.
- Test noexcept conditions for move operations so containers like std::vector behave efficiently.
- Consider array-specialization (UniquePtr<T[]>), which must use delete[] and provide operator[].

If you want, I can:
- Provide a fully compilable header-only implementation including SFINAE constraints for the converting constructor and conditional noexcept specifications.
- Add unit tests that demonstrate correct behavior (move, release, reset, custom deleter, polymorphic conversion).

Which would you prefer: the full correct implementation (with SFINAE and noexcept conditionals) or the simple single-object version plus tests?