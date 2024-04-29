add_executable(NetworkEventDispatcherTest communication/NetworkEventDispatcherTest.cpp)
target_link_libraries(NetworkEventDispatcherTest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(NetworkEventDispatcherTest PRIVATE shared_lib)

add_executable(NetworkMessageTest communication/NetworkMessageTest.cpp)
target_link_libraries(NetworkMessageTest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(NetworkMessageTest PRIVATE shared_lib)

add_executable(AnimationDataTest core/animations/AnimationDataTest.cpp)
target_link_libraries(AnimationDataTest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(AnimationDataTest PRIVATE shared_lib)

add_executable(WeaponTest core/entities/WeaponTest.cpp)
target_link_libraries(WeaponTest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(WeaponTest PRIVATE shared_lib)

add_executable(MapBuilderTest core/map/MapBuilderTest.cpp)
target_link_libraries(MapBuilderTest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(MapBuilderTest PRIVATE shared_lib)
target_link_libraries(MapBuilderTest PRIVATE shared_lib_testing_framework)

add_executable(MapTest core/map/MapTest.cpp)
target_link_libraries(MapTest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(MapTest PRIVATE shared_lib)

add_executable(CalcTest core/math/CalcTest.cpp)
target_link_libraries(CalcTest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(CalcTest PRIVATE shared_lib)

add_executable(ObservableTest core/utility/ObservableTest.cpp)
target_link_libraries(ObservableTest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(ObservableTest PRIVATE shared_lib)

add_test(NetworkEventDispatcherTest NetworkEventDispatcherTest)
add_test(NetworkMessageTest NetworkMessageTest)
add_test(AnimationDataTest WeaponTest)
add_test(WeaponTest WeaponTest)
add_test(MapBuilderTest MapBuilderTest)
add_test(MapTest MapTest)
add_test(CalcTest CalcTest)
add_test(ObservableTest ObservableTest)
