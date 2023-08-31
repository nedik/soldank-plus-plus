
add_executable(CalcTest CalcTest.cpp)
target_link_libraries(CalcTest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(CalcTest PRIVATE client_lib)

add_test(math_gtests CalcTest)