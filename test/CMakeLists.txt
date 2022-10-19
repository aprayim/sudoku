include(GoogleTest)

macro(add_test name)
    add_executable(test_${name} test_${name}.cc)
    target_link_libraries(test_${name} board GTest::gtest_main)
    gtest_discover_tests(test_${name})
endmacro()

add_test(AllowedValues)