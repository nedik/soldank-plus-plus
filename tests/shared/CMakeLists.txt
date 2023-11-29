add_executable(CommunicationTest communication/NetworkMessageTest.cpp)
target_link_libraries(CommunicationTest PRIVATE gtest gtest_main gmock gmock_main)
target_link_libraries(CommunicationTest PRIVATE shared_lib)

add_executable(CalcTest core/math/CalcTest.cpp)
target_link_libraries(CalcTest PRIVATE gtest gtest_main gmock gmock_main)
target_link_libraries(CalcTest PRIVATE shared_lib)

add_executable(WeaponTest core/entities/WeaponTest.cpp)
target_link_libraries(WeaponTest PRIVATE gtest gtest_main gmock gmock_main)
target_link_libraries(WeaponTest PRIVATE shared_lib)

add_test(communication_gtests CommunicationTest)
add_test(math_gtests CalcTest)
add_test(entities_gtests WeaponTest)
