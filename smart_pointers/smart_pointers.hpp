#include <cstddef>
#include <memory>
#include <type_traits>

namespace CustomSmartPointers {
    //////////////////// UNIQUE PTR ////////////////////////////
    // this deleter is used to delete the object
    template <class T, class Deleter=std::default_delete<T>>
    class UniquePtr {
    public: 
        using value_type = T; 
        using pointer = T*; 
        ///////////////// DESTRUCTORS ///////////////////
        ~UniquePtr(); 
        ///////////////// CONSTRUCTORS //////////////////
        constexpr UniquePtr() noexcept;
        constexpr UniquePtr(std::nullptr_t) noexcept;  
        explicit UniquePtr(pointer p) noexcept;
        // unique_ptr( pointer p, /* see below */ d1 ) noexcept; 
        // unique_ptr( pointer p, /* see below */ d2 ) noexcept;
        template <class E> 
        UniquePtr(UniquePtr<T, E>&& u) noexcept;
        // The class should be MoveAssignable and MoveConstructible
        // only movable if the unique_ptr is not const
        // The class should not be CopyAssignable and CopyConstructible
        UniquePtr(UniquePtr&) = delete; 
        UniquePtr(const UniquePtr&) = delete; 
        UniquePtr(UniquePtr&& other_ptr); 
        UniquePtr(const UniquePtr&& other_ptr) = delete; 
        //////////////// ASSIGNMENTS ////////////////////
        UniquePtr& operator=(UniquePtr&) = delete; 
        UniquePtr& operator=(const UniquePtr&) = delete; 
        UniquePtr& operator=(UniquePtr&&); 
        UniquePtr& operator=(const UniquePtr&& other_ptr) = delete; 

        //////////////// MEMBER FUNCTIONS //////////////////
        UniquePtr& operator=(std::nullptr_t) noexcept; 

        //////////////// MODIFIERS /////////////////////
        /**
            releases the managed ownership and then get() returns 
            nullptr
        */
        pointer release() noexcept;
        /** 
        assigns new value to the ptr
        */ 
        void reset(pointer ptr = pointer()) noexcept; 
        template<class U>
        void reset(U ptr) noexcept; 
        void reset(std::nullptr_t = nullptr) noexcept; 
        /** swaps with another uniqueptr */
        void swap(UniquePtr& other) noexcept; 

        /////////////////// GETTERS //////////////////////
        pointer get() const noexcept;
        Deleter& get_deleter() noexcept; 
        const Deleter& get_deleter() const noexcept;  
        explicit operator bool() const noexcept; 
        typename std::add_lvalue_reference<T>::type operator*() const noexcept(noexcept(*std::declval<pointer>())); 
        pointer operator->() const noexcept;
        // provides access to elements of array managed by a unique_ptr 
        T& operator[](std::size_t i) const; 
    private: 
        pointer ptr; 
        Deleter deleter; 
    }; 
    /** 
    * About Unique ptr
    * only non const unique ptr can have ownership passed
    * 
    */
    ///////////////////// DESTRUCTORS ////////////////
    template<class T, class Deleter>
    UniquePtr<T, Deleter>::~UniquePtr() {
        if(ptr!=nullptr) {
            deleter(ptr); 
        }
    }
    ///////////////////// CONSTRUCTORS //////////////
    template<class T, class Deleter>
    constexpr UniquePtr<T, Deleter>::UniquePtr() noexcept
        : ptr(nullptr), deleter() {}

    template<class T, class Deleter>
    constexpr UniquePtr<T, Deleter>::UniquePtr(std::nullptr_t) noexcept
        : ptr(nullptr), deleter() {}

    template<class T, class Deleter>
    UniquePtr<T, Deleter>::UniquePtr(pointer p) noexcept
        : ptr(p), deleter() {}

    template <class T, class Deleter>
    template <class E> 
    UniquePtr<T, Deleter>::UniquePtr(UniquePtr<T, E>&& u) noexcept
        : ptr(u.ptr),
        // here std::forward is needed 
        // because if u.get_deleter() passed directly
        // it would take it as lvalue reference, calling copy 
        // breaks move semantics
        // making this constructor expensive  
        deleter(std::forward<E>(u.get_deleter())) 
    {}
    
    // Value type constructor cannot exist for unique_ptr
    // this was for array initiliazation
    // template <class T, class Deleter>
    // UniquePtr<T, Deleter>::UniquePtr(value_type p) noexcept {
    //     ptr = &p; 
    //     deleter = std::default_delete<T>();  
    // }
    ///////////////////// ASSIGNMENTS //////////////////
    template<class T, class Deleter>
    UniquePtr<T, Deleter>& UniquePtr<T, Deleter>::operator=(UniquePtr&& ptr1) {
        if(ptr == ptr1) {
            return ptr1; 
        } 
        ptr = ptr1; 
    }
    ///////////////////// MEMBER FUNCTIONS ////////////////
    template<class T, class Deleter>
    UniquePtr<T, Deleter>& UniquePtr<T, Deleter>::operator=(std::nullptr_t) noexcept {
        get_deleter()(ptr); 
    } 
    /////////////////// GETTERS //////////////////////
    template<class T, class Deleter>
    UniquePtr<T, Deleter>::pointer UniquePtr<T, Deleter>::get() const noexcept {
        return ptr; 
    }

    template<class T, class Deleter>
    Deleter& UniquePtr<T, Deleter>::get_deleter() noexcept {
        return deleter; 
    }

    template<class T, class Deleter>  
    UniquePtr<T, Deleter>::operator bool() const noexcept {
        return ptr != nullptr; 
    } 

    template<class T, class Deleter>
    typename std::add_lvalue_reference<T>::type UniquePtr<T, Deleter>::operator*() 
        const noexcept(noexcept(*std::declval<pointer>())) 
    {
        return *ptr; 
    } 
    template<class T, class Deleter>
    UniquePtr<T, Deleter>::pointer UniquePtr<T, Deleter>::operator->() const noexcept {
        return ptr; 
    }
    // provides access to elements of array managed by a unique_ptr 
    template<class T, class Deleter>
    T& UniquePtr<T, Deleter>::operator[](std::size_t i) const {
        return ptr; 
    } 

    /////////////////////////////////////////////////////////
}; 

