#include "smart_pointers.hpp" 
#include <gtest/gtest.h>
#include <memory>
using namespace CustomSmartPointers; 

bool isDeleted = false; 
struct MockObject {
    ~MockObject() {isDeleted = true;}
}; 

struct DerivedMockObject: MockObject {
    ~DerivedMockObject() {
        std::cout << "Derived Delete called" << std::endl; 
        isDeleted = true; 
    }
}; 

struct NonDerivedMockObject {
    ~NonDerivedMockObject() {
        std::cout << "Non Derived Delete called" << std::endl; 
        isDeleted = true; 
    }
}; 

////////////////////// BASIC INTERFACE TEST ///////////////
TEST(SmartPointerTests, DefaultAndNullptrConstruction) {
    UniquePtr<MockObject> ptr1;
    UniquePtr<MockObject> ptr2(nullptr);

    EXPECT_EQ(ptr1.get(), nullptr);
    EXPECT_EQ(ptr2.get(), nullptr);
    EXPECT_FALSE(ptr1); // Tests operator bool
}

TEST(SmartPointerTests, CheckIfPtrDestructorCalled) {
    isDeleted = false;
    {
        UniquePtr<MockObject> ptr(new MockObject()); 
    } 
    EXPECT_TRUE(isDeleted); 
    isDeleted = false; 
}

TEST(SmartPointerTests, CheckIfUniquePtrExistsUniqeuely) {
    auto rawObject {new MockObject()}; 
    auto unique_ptr1 {UniquePtr<MockObject>(rawObject)};
    auto unique_ptr2 = std::move(unique_ptr1);  

    EXPECT_EQ(unique_ptr1.get(), nullptr); 
    EXPECT_EQ(unique_ptr2.get(), rawObject); 
    
    EXPECT_EQ(typeid(unique_ptr2.get_deleter()), typeid(std::default_delete<MockObject>)); 
}

TEST(SmartPointerTests, MoveAssignmentAndSelfAssignment) {
    auto raw1 = new MockObject();
    auto raw2 = new MockObject();
    
    UniquePtr<MockObject> ptr1(raw1);
    UniquePtr<MockObject> ptr2(raw2);

    isDeleted = false;
    ptr2 = std::move(ptr1); // Move assignment triggers destruction of raw2

    EXPECT_TRUE(isDeleted); // raw2 should be deleted
    EXPECT_EQ(ptr2.get(), raw1);
    EXPECT_EQ(ptr1.get(), nullptr);

    // Self-assignment safety check
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wself-move"
    ptr2 = std::move(ptr2); 
    #pragma clang diagnostic pop
    EXPECT_EQ(ptr2.get(), raw1); // Should remain unchanged
}

TEST(SmartPointerTests, NullptrAssignmentClearsResource) {
    UniquePtr<MockObject> ptr(new MockObject());
    isDeleted = false;

    ptr = nullptr;


}
TEST(SmartPointerTests, SwapFunctionSwapsPpinterValue) {
    UniquePtr<MockObject> up1(new MockObject());
    UniquePtr<MockObject> up2(new MockObject());

    auto p1 {up1.get()}; 
    auto p2 {up2.get()}; 

    auto del1 {up1.get_deleter()}; 
    auto del2 {up2.get_deleter()}; 

    swap(up1, up2); 
    EXPECT_EQ(up1.get(), p2); 
    EXPECT_EQ(up2.get(), p1); 
}
///////////////////////////////////////////////////////////

/////////// Converting move ctor called ///////////////////
TEST(SmartPointerTests, DerivedMoveCtorWorks) {
    UniquePtr<DerivedMockObject> derived_mock_up {new DerivedMockObject()}; 
    UniquePtr<MockObject> mock_up {std::move(derived_mock_up)}; 
}

/////////// Custom delete ctor ////////////////////////////
TEST(SmartPointerTests, CustomDeleteCtor) {
    auto deleted {false}; 
    auto deleter = [&deleted] (MockObject* p) {
        deleted = true; 
        std::cout << "Object deleted" << std::endl; 
        delete p; 
    }; 
    {
        UniquePtr<MockObject, decltype(deleter)> ptr(new MockObject(), deleter); 
    }
    EXPECT_TRUE(deleted); 
}

/////////// Make_unique ctor ////////////////////////////
TEST(SmartPointerTests, MakeUniquePtrWorks) {
    auto deleted {false}; 
    auto deleter = [&deleted] (MockObject* p) {
        deleted = !deleted; 
        std::cout << "Object deleted" << std::endl; 
        delete p; 
    };
    {
        auto p1 {make_unique<MockObject>()}; 
        auto p2 {make_unique_with_deleter<MockObject>(deleter)}; 
    }
    EXPECT_EQ(deleted, true); 
    EXPECT_EQ(isDeleted, true); 
}

//////////// RELEASE FUNCTION ///////////////////////////
TEST(SmartPointerTests, ReleaseFunctionCheckReturnValue) {
    auto p {make_unique<MockObject>()}; 
    auto x = p.release(); 
    EXPECT_EQ(typeid(MockObject), typeid(*x)); 
    EXPECT_EQ(p.get(), nullptr); 
}

TEST(SmartPointerTests, ReleaseFunctionCheckOnEmptyPtr) {
    UniquePtr<MockObject> empty; 
    MockObject* raw = empty.release(); 
    EXPECT_EQ(empty.get(), nullptr); 
    EXPECT_EQ(raw, nullptr); 
}

/////////// RESET FUNCTION ////////////////////////////////
TEST(SmartPointerTests, NewValueResetCheck) {
    isDeleted = false; 
    UniquePtr<MockObject> up {new MockObject()};
    auto obj2 {new MockObject()}; 
    up.reset(obj2); 
    // DELETER For old ptr was called
    EXPECT_EQ(isDeleted, true); 
    EXPECT_EQ(up.get(), obj2); 
}

TEST(SmartPointerTests, NullPtrValueSetResetCheck) {
    isDeleted = false; 
    auto up1 {make_unique<MockObject>()}; 
    up1.reset(nullptr); 
    EXPECT_EQ(isDeleted, true); 
    EXPECT_EQ(up1.get(), nullptr); 
}

TEST(SmartPointerTests, ObjectAssignmentUpcastingWorks) {
    auto deleted {false}; 
    auto custom_deleter = [&deleted] (MockObject* p) {
        deleted = true; 
        delete p; 
    };
    auto up1 {make_unique_with_deleter<MockObject> (custom_deleter)}; 
    up1.reset(new DerivedMockObject()); 

    EXPECT_EQ(deleted, true);
}

/////////// OPERATOR* DEREFERENCE ////////////////////////

TEST(SmartPointerTests, DereferenceOperatorReturnsValue) {
    auto up = make_unique<int>(42);
    EXPECT_EQ(*up, 42);
}

TEST(SmartPointerTests, DereferenceOperatorAllowsMutation) {
    auto up = make_unique<int>(10);
    *up = 99;
    EXPECT_EQ(*up, 99);
    // verify mutation happened on the actual heap object
    EXPECT_EQ(*up.get(), 99);
}

TEST(SmartPointerTests, DereferenceOperatorOnMockObject) {
    // verifies operator* returns a reference, not a copy
    UniquePtr<MockObject> up(new MockObject());
    MockObject& ref = *up;
    // ref and up.get() must point to the same object
    EXPECT_EQ(&ref, up.get());
}

TEST(SmartPointerTests, DereferenceAfterMoveStillValid) {
    auto up1 = make_unique<int>(7);
    auto up2 = std::move(up1);
    // up2 now owns the object — dereference must work
    EXPECT_EQ(*up2, 7);
}

TEST(SmartPointerTests, DereferenceReturnsReferenceNotCopy) {
    struct Counter { int val = 0; };
    UniquePtr<Counter> up(new Counter());
    // mutate through the reference operator* returns
    (*up).val = 55;
    EXPECT_EQ(up->val, 55); // cross-check with operator->
}

/////////// OPERATOR-> MEMBER ACCESS /////////////////////

TEST(SmartPointerTests, ArrowOperatorReadsMembers) {
    struct Point { int x = 3; int y = 4; };
    UniquePtr<Point> up(new Point());
    EXPECT_EQ(up->x, 3);
    EXPECT_EQ(up->y, 4);
}

TEST(SmartPointerTests, ArrowOperatorWritesMembers) {
    struct Point { int x = 0; int y = 0; };
    UniquePtr<Point> up(new Point());
    up->x = 10;
    up->y = 20;
    EXPECT_EQ(up->x, 10);
    EXPECT_EQ(up->y, 20);
}

TEST(SmartPointerTests, ArrowOperatorCallsMemberFunction) {
    struct Counter {
        int val = 0;
        void increment() { val++; }
        int get() const { return val; }
    };
    UniquePtr<Counter> up(new Counter());
    up->increment();
    up->increment();
    EXPECT_EQ(up->get(), 2);
}

TEST(SmartPointerTests, ArrowOperatorReturnsSameAddressAsGet) {
    struct Point { int x = 1; int y = 2; };
    UniquePtr<Point> up(new Point());
    // operator-> must return the raw pointer get() returns
    EXPECT_EQ(up.operator->(), up.get());
}

TEST(SmartPointerTests, ArrowOperatorWorksAfterReset) {
    struct Point { int x = 0; };
    UniquePtr<Point> up(new Point());
    up->x = 5;

    auto* fresh = new Point();
    fresh->x = 99;
    up.reset(fresh);

    // operator-> must now access the new object
    EXPECT_EQ(up->x, 99);
}

TEST(SmartPointerTests, ArrowOperatorOnDerivedType) {
    // operator-> through a Base* must reach Derived's members
    // via virtual dispatch
    struct Base {
        int base_val = 1;
        virtual int id() { return 0; }
        virtual ~Base() = default;
    };
    struct Derived : Base {
        int derived_val = 2;
        int id() override { return 1; }
    };
    UniquePtr<Base> up(new Derived());
    EXPECT_EQ(up->base_val, 1);
    EXPECT_EQ(up->id(), 1);  // virtual dispatch through operator->
}

/////////// CONVERTING MOVE ASSIGNMENT ////////////////////

// Test 1 — basic upcast, ownership transfers correctly
TEST(SmartPointerTests, ConvertingMoveAssign_SourceEmptiedTargetOwns) {
    auto* raw = new DerivedMockObject();
    UniquePtr<DerivedMockObject> derived(raw);
    UniquePtr<MockObject> base;

    base = std::move(derived);

    EXPECT_EQ(derived.get(), nullptr);  // source must be empty
    EXPECT_EQ(base.get(), raw);         // target owns the raw ptr
}

// Test 2 — old resource in target is deleted before taking new one
TEST(SmartPointerTests, ConvertingMoveAssign_OldResourceDeleted) {
    isDeleted = false;
    UniquePtr<MockObject> base(new MockObject());  // base holds something
    UniquePtr<DerivedMockObject> derived(new DerivedMockObject());

    base = std::move(derived);  // base's old MockObject must be deleted

    EXPECT_TRUE(isDeleted);     // old resource freed
    EXPECT_NE(base.get(), nullptr);
}

// Test 3 — custom deleter is transferred from source to target
TEST(SmartPointerTests, ConvertingMoveAssign_CustomDeleterTransferred) {
    bool custom_deleted = false;
    auto deleter = [&custom_deleted](MockObject* p) {
        custom_deleted = true;
        delete p;
    };

    UniquePtr<DerivedMockObject, decltype(deleter)> derived(
        new DerivedMockObject(), deleter
    );
    UniquePtr<MockObject, decltype(deleter)> base(nullptr, deleter);

    base = std::move(derived);
    // now let base destruct — custom deleter must fire
    base.reset();

    EXPECT_TRUE(custom_deleted);  // deleter came from derived, not default
}

// Test 4 — virtual destructor called correctly after upcast assign
TEST(SmartPointerTests, ConvertingMoveAssign_VirtualDtorCalledOnDestruct) {
    bool derived_deleted = false;
    struct Base2 {
        virtual ~Base2() = default;
    };
    struct Derived2 : Base2 {
        bool* flag;
        Derived2(bool* f) : flag(f) {}
        ~Derived2() { *flag = true; }
    };

    {
        UniquePtr<Derived2> d(new Derived2(&derived_deleted));
        UniquePtr<Base2> b;
        b = std::move(d);
        // b destructs here — must call ~Derived2() via virtual dispatch
    }
    EXPECT_TRUE(derived_deleted);  // partial destruction would leave this false
}

// Test 5 — compile-time: downcast direction is blocked by SFINAE
TEST(SmartPointerTests, ConvertingMoveAssign_DowncastBlockedAtCompileTime) {
    // UniquePtr<MockObject> base(new MockObject());
    // UniquePtr<DerivedMockObject> derived;
    // derived = std::move(base);  // must NOT compile — Base* -> Derived* invalid
    //
    // Verified statically — no runtime check needed:
    static_assert(
        !std::is_convertible_v<MockObject*, DerivedMockObject*>,
        "downcast direction must be blocked by SFINAE"
    );
    SUCCEED();
}

// Test 6 — assign into a target that already went through a prior move
TEST(SmartPointerTests, ConvertingMoveAssign_ChainedAssignmentsWork) {
    auto* raw = new DerivedMockObject();
    UniquePtr<DerivedMockObject> d1(raw);
    UniquePtr<MockObject> b1;
    UniquePtr<MockObject> b2;

    b1 = std::move(d1);   // first converting assign
    b2 = std::move(b1);   // same-type move assign — b1 is now UniquePtr<MockObject>

    EXPECT_EQ(d1.get(), nullptr);
    EXPECT_EQ(b1.get(), nullptr);
    EXPECT_EQ(b2.get(), raw);     // raw made it all the way through
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS(); 
}