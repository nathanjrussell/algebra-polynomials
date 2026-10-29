add_executable(algebra_polynomials_test polynomial_test.cpp)
target_link_libraries(algebra_polynomials_test PRIVATE algebra::polynomials algebra::rings GTest::gtest_main)
gtest_discover_tests(algebra_polynomials_test TEST_PREFIX "algebra_polynomials.")
