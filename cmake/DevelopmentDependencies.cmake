include(FetchContent)
if(NOT TARGET njr::algebra_rings)
    FetchContent_Declare(
        njr_algebra_rings
        GIT_REPOSITORY git@github.com:nathanjrussell/algebra-rings.git
        GIT_TAG f789306ddf2b9375dd00a92d476082831439f145
    )
    FetchContent_MakeAvailable(njr_algebra_rings)
endif()
