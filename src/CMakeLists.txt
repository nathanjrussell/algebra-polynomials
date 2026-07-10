add_library(njr_algebra_polynomials INTERFACE)
add_library(njr::algebra_polynomials ALIAS njr_algebra_polynomials)
target_compile_features(njr_algebra_polynomials INTERFACE cxx_std_20)
target_include_directories(njr_algebra_polynomials INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)
target_link_libraries(njr_algebra_polynomials INTERFACE
    njr::algebra_core
)
