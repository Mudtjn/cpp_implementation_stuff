#include "smart_pointers.hpp" 
#include <gtest/gtest.h>
#include <memory>
using namespace CustomSmartPointers; 

bool isDeleted = false; 
struct MockObject {
    ~MockObject() {isDeleted = true;}
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

    EXPECT_TRUE(isDeleted);
    EXPECT_EQ(ptr.get(), nullptr);
}
///////////////////////////////////////////////////////////

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS(); 
}