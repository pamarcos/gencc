#include "gencc.h"
#include "gtest/gtest.h"

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(FirstTest, test)
{
    EXPECT_TRUE(true);
}
