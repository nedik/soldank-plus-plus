add_executable(CalcTest core/math/CalcTest.cpp)
target_link_libraries(CalcTest PRIVATE gtest gtest_main gmock gmock_main)
target_link_libraries(CalcTest PRIVATE client_lib)

add_executable(WeaponTest core/entities/WeaponTest.cpp)
target_link_libraries(WeaponTest PRIVATE gtest gtest_main gmock gmock_main)
target_link_libraries(WeaponTest PRIVATE client_lib)

add_test(math_gtests CalcTest)
add_test(entities_gtests WeaponTest)
