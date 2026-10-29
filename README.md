# Algebra polynomials

Polynomial arithmetic over typed ring elements.

```cmake
include(FetchContent)

FetchContent_Declare(
    algebra_polynomials
    GIT_REPOSITORY git@github.com:nathanjrussell/algebra-polynomials.git
    GIT_TAG v0.2.0
)

FetchContent_MakeAvailable(algebra_polynomials)

target_link_libraries(my_app PRIVATE algebra::polynomials)
```
