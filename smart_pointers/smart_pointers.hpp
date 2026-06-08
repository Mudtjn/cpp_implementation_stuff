#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

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
        // If no except no try catch, the function directly 
        // throws std::terminate
        UniquePtr(pointer p, Deleter d) noexcept(std::is_nothrow_default_constructible_v<Deleter>); 
        constexpr UniquePtr() noexcept;
        constexpr UniquePtr(std::nullptr_t) noexcept;  
        explicit UniquePtr(pointer p) noexcept(std::is_nothrow_default_constructible_v<Deleter>);
        // unique_ptr( pointer p, /* see below */ d1 ) noexcept; 
        // unique_ptr( pointer p, /* see below */ d2 ) noexcept;
        
        // to make this constructor convertible 
        // and add checks , need template on source pointer type as well 
        template <class U, class E, 
            std::enable_if_t<std::is_convertible_v<U*, T*> &&
            std::is_constructible_v<Deleter, E&&>
            , int> = 0
        > 
        UniquePtr(UniquePtr<U, E>&& u) noexcept (
            std::is_nothrow_constructible_v<Deleter, E&&>
        ); 
        // The class should be MoveAssignable and MoveConstructible
        // only movable if the unique_ptr is not const
        // The class should not be CopyAssignable and CopyConstructible
        UniquePtr(UniquePtr&) = delete; 
        UniquePtr(const UniquePtr&) = delete; 
        // UniquePtr(UniquePtr&& other_ptr); -> should be noexcept
        UniquePtr(UniquePtr&& other) noexcept; 
        UniquePtr(const UniquePtr&& other_ptr) = delete; 
        //////////////// ASSIGNMENTS ////////////////////
        UniquePtr& operator=(UniquePtr&) = delete; 
        UniquePtr& operator=(const UniquePtr&) = delete; 
        UniquePtr& operator=(UniquePtr&&) noexcept; 
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
        void reset(std::nullptr_t) noexcept;  
        /** swaps with another uniqueptr */
        void swap(UniquePtr& other) noexcept; 

        /////////////////// GETTERS //////////////////////
        pointer get() const noexcept;
        Deleter& get_deleter() noexcept; 
        const Deleter& get_deleter() const noexcept;  
        explicit operator bool() const noexcept; 
        T& operator*() const noexcept(noexcept(*std::declval<pointer>())); 
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
    UniquePtr<T, Deleter>::UniquePtr(pointer p, Deleter d) 
    noexcept(std::is_nothrow_default_constructible_v<Deleter>) 
        : ptr(p), deleter(std::move(d)) {}

    template<class T, class Deleter>
    constexpr UniquePtr<T, Deleter>::UniquePtr() noexcept
        : ptr(nullptr), deleter() {}

    template<class T, class Deleter>
    constexpr UniquePtr<T, Deleter>::UniquePtr(std::nullptr_t) noexcept
        : ptr(nullptr), deleter() {}

    template<class T, class Deleter>
    UniquePtr<T, Deleter>::UniquePtr(pointer p) 
    noexcept(std::is_nothrow_default_constructible_v<Deleter>)
        : ptr(p), deleter() {}

    template <class T, class Deleter>
    template <class U, class E, 
            std::enable_if_t<std::is_convertible_v<U*, T*> &&
            std::is_constructible_v<Deleter, E&&>
            , int>>  
    UniquePtr<T, Deleter>::UniquePtr(UniquePtr<U, E>&& u) noexcept (
            std::is_nothrow_constructible_v<Deleter, E&&>
    ) 
        : ptr(u.release()),
        deleter(std::move(u.get_deleter()))
    {}

    template<class T, class Deleter>
    UniquePtr<T, Deleter>::UniquePtr(UniquePtr&& other) noexcept 
        : ptr(other.release()), deleter(std::move(other.deleter))
    {}
    // Value type constructor cannot exist for unique_ptr
    // this was for array initiliazation
    // template <class T, class Deleter>
    // UniquePtr<T, Deleter>::UniquePtr(value_type p) noexcept {
    //     ptr = &p; 
    //     deleter = std::default_delete<T>();  
    // }
    ///////////////////// ASSIGNMENTS //////////////////
    // INITIAL IMPLEMENTATION
    // template<class T, class Deleter>
    // UniquePtr<T, Deleter>& UniquePtr<T, Deleter>::operator=(UniquePtr&& ptr1) {
    //     if(ptr == ptr1) { -> compares ptr (T*) to ptr1 (UniquePtr) 
    //         return ptr1; -> return *this is right option 
    //     } 
    //     ptr = ptr1; -> this assignment does not work 
    // move or copy deleter from ptr1 not done
    //     ptr1.get_deleter()(ptr1); 
    //     ptr1 = nullptr; 
    // }
    template<class T, class Deleter>
    UniquePtr<T, Deleter>& UniquePtr<T, Deleter>::operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            reset(other.release());
            
            // deleter = std::move(other.deleter); 
            // this had to be commented out as the 
            // default lambda taking values is not default copy assignable
            // and hence implicity not move assignable
            
            deleter.~Deleter(); 
            new (&deleter) Deleter(std::move(other.deleter)); 
        } 
        return *this; 
    }
    ///////////////////// MEMBER FUNCTIONS ////////////////
    template<class T, class Deleter>
    UniquePtr<T, Deleter>& UniquePtr<T, Deleter>::operator=(std::nullptr_t) noexcept {
        //  wrong implementation
        // get_deleter()(ptr); 
        reset(nullptr); 
        return *this;
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
    const Deleter& UniquePtr<T, Deleter>::get_deleter() const noexcept {
        return deleter; 
    }

    template<class T, class Deleter>  
    UniquePtr<T, Deleter>::operator bool() const noexcept {
        return ptr != nullptr; 
    } 

    template<class T, class Deleter>
    T& UniquePtr<T, Deleter>::operator*() 
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
        return ptr[i]; 
    } 

    //////////////// MODIFIERS /////////////////////
    /**
        releases the managed ownership and then get() returns 
        nullptr
    */
    template<class T, class Deleter>
    UniquePtr<T, Deleter>::pointer UniquePtr<T, Deleter>::release() noexcept {
        pointer return_ptr = ptr; 
        ptr = nullptr; 
        return return_ptr; 
    }
    /** 
    assigns new value to the ptr
    */ 
    template<class T, class Deleter>
    void UniquePtr<T, Deleter>::reset(pointer other_ptr) noexcept {
        pointer old_ptr = ptr; 
        ptr = other_ptr; 
        if(old_ptr != nullptr) {
            get_deleter()(old_ptr); 
        }
    }
    template<class T, class Deleter>
    template<class U>
    void UniquePtr<T, Deleter>::reset(U ptr) noexcept {
        reset(static_cast<pointer>(ptr)); 
    } 
    template<class T, class Deleter>
    void UniquePtr<T, Deleter>::reset(std::nullptr_t) noexcept {
        reset(pointer()); 
    } 
    /** swaps with another uniqueptr */
    template<class T, class Deleter>
    void UniquePtr<T, Deleter>::swap(UniquePtr& other) noexcept {
        std::swap(ptr, other.ptr); 
        std::swap(deleter, other.deleter);     
    }

    /////////// NON-MEMBER FUNCTIONS /////////////////////////
    // usuaully 2 required , one with deleter defined 
    // other deleter implicit 
    template <class T, class... Args>
    UniquePtr<T, std::default_delete<T>> make_unique(Args&&... args) {
        return UniquePtr<T, std::default_delete<T>> (new T(std::forward<Args>(args)...)); 
    }

    template <class T, class Deleter, class... Args>
    UniquePtr<T, Deleter> make_unique_with_deleter(Deleter d, Args&&... args) {
        return UniquePtr<T, Deleter>(new T(std::forward<Args>(args)...), std::move(d)); 
    }
    /////////////////////////////////////////////////////////

    ///////////////////// CUSTOM SWAP IMPLEMENTATION //////////////
    template <class T, class Deleter>
    void swap(UniquePtr<T, Deleter>& a, UniquePtr<T, Deleter>& b) noexcept {
        a.swap(b); 
    } 
}; 

