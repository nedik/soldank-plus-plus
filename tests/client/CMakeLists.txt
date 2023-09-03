add_executable(CalcTest CalcTest.cpp)
target_link_libraries(CalcTest PRIVATE gtest gtest_main gmock gmock_main)
target_link_libraries(CalcTest PRIVATE client_lib)

add_test(math_gtests CalcTest)
