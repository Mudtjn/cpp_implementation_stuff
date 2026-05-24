#include "smart_pointers.hpp" 
#include <gtest/gtest.h>
using namespace CustomSmartPointers; 

bool isDeleted = false; 
struct MockObject {
    ~MockObject() {isDeleted = true;}
}; 

////////////////////// BASIC INTERFACE TEST ///////////////
TEST(SmartPointerTests, CheckIfPtrDestructorCalled) {
    isDeleted = false;
    {
        UniquePtr<MockObject> ptr(new MockObject()); 
    } 
    EXPECT_TRUE(isDeleted); 
}

TEST(SmartPointerTests, CheckIfUniquePtrExistsUniqeuely) {
    auto rawObject {new MockObject()}; 
    auto unique_ptr1 {UniquePtr<MockObject>(rawObject)};
    auto unique_ptr2 = std::move(unique_ptr1);  

    EXPECT_EQ(unique_ptr1.get(), nullptr); 
    EXPECT_EQ(unique_ptr2.get(), rawObject); 
}
///////////////////////////////////////////////////////////

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS(); 
}