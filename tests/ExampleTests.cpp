#include <gtest/gtest.h>

struct ExampleTests
    : public ::testing::Test
{
    int *x;

    int GetX()
    {
        return *x;
    }

    virtual void SetUp() override
    {
        fprintf(stderr, "");
        x = new int(10);
    }

    virtual void TearDown() override
    {
        delete x;
    }
};

bool f()
{
    return true;
}

TEST_F(ExampleTests, DemoGTestMacros)
{
    const bool result = f();
    EXPECT_EQ(true, result);
    EXPECT_TRUE(true);
    EXPECT_EQ(10, GetX()) << "\bSomething Wrong!";
}

TEST(ExampleTests2, Something)
{
    EXPECT_TRUE(true);
}
