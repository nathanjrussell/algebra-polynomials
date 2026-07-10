# Algebra polynomials

Polynomial arithmetic over typed ring elements.

```cmake
include(FetchContent)

FetchContent_Declare(
    njr_algebra_polynomials
    GIT_REPOSITORY git@github.com:nathanjrussell/algebra-polynomials.git
    GIT_TAG v0.1.0
)

FetchContent_MakeAvailable(njr_algebra_polynomials)

target_link_libraries(my_app PRIVATE njr::algebra_polynomials)
```
