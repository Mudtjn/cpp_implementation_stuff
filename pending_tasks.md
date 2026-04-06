You’re pretty close for a toy vector, but there are several correctness bugs and a lot of standard std::vector semantics that aren’t followed.

Semantic / API mismatches vs std::vector

No insert, erase, clear, assign, emplace_* members, which are standard modifiers.

Iterator-related issues
You define reverse_iterator types but provide no rbegin(), rend(), crbegin(), crend() members, which are standard.

You only provide cbegin()/cend(), but not begin() const / end() const; usually you have both const and non-const overloads.

Exception safety and construction
All allocations use new T[...] but you never handle allocation failure or exceptions during element copy/construction; std::vector has strong guarantees for many operations when possible.

Constructors taking int size should use an unsigned size_type and probably value-initialize elements (new T[size]() or std::uninitialized_value_construct_n in a more advanced implementation).

Concrete fixes you can do next
Add proper size_type, reference, const_reference typedefs and fix all element access to return references.
