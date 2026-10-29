include(FetchContent)
if(NOT TARGET algebra::rings)
    FetchContent_Declare(
        algebra_rings
        GIT_REPOSITORY git@github.com:nathanjrussell/algebra-rings.git
        GIT_TAG 2a11ac8b8ea89d9ccf2ce551a8512dde213fde50
    )
    FetchContent_MakeAvailable(algebra_rings)
endif()
