#include "core/math/Calc.hpp"
#include <gtest/gtest.h>

TEST(CalcTests, TestDistance)
{
    float expected = 5.0;
    float actual = Soldat::Calc::Distance({ 0.0, 0.0 }, { 3.0, 4.0 });
    ASSERT_FLOAT_EQ(expected, actual);
    expected = 0.0;
    actual = Soldat::Calc::Distance({ 4.5668F, -2.60855F }, { 4.5668F, -2.60855F });
    ASSERT_FLOAT_EQ(expected, actual);
    expected = 6.979953;
    actual = Soldat::Calc::Distance({ 4.5668F, -2.60855F }, { 1.05F, -8.6378F });
    ASSERT_FLOAT_EQ(expected, actual);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}