add_executable(NetworkEventDispatcherTest communication/NetworkEventDispatcherTest.cpp)
target_link_libraries(NetworkEventDispatcherTest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(NetworkEventDispatcherTest PRIVATE shared_lib)

add_executable(NetworkMessageTest communication/NetworkMessageTest.cpp)
target_link_libraries(NetworkMessageTest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(NetworkMessageTest PRIVATE shared_lib)

add_executable(CalcTest core/math/CalcTest.cpp)
target_link_libraries(CalcTest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(CalcTest PRIVATE shared_lib)

add_executable(WeaponTest core/entities/WeaponTest.cpp)
target_link_libraries(WeaponTest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(WeaponTest PRIVATE shared_lib)

add_test(communication_gtests NetworkEventDispatcherTest NetworkMessageTest)
add_test(math_gtests CalcTest)
add_test(entities_gtests WeaponTest)
