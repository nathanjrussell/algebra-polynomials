add_library(algebra_polynomials INTERFACE)
add_library(algebra::polynomials ALIAS algebra_polynomials)
target_compile_features(algebra_polynomials INTERFACE cxx_std_20)
target_include_directories(algebra_polynomials INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)
target_link_libraries(algebra_polynomials INTERFACE
    algebra::core
)
