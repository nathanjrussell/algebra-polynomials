include(FetchContent)
if(NOT TARGET algebra::core)
    FetchContent_Declare(
        algebra_core
        GIT_REPOSITORY git@github.com:nathanjrussell/algebra-core.git
        GIT_TAG df491678e274372fb0a08914c61a4d504414a739
    )
    FetchContent_MakeAvailable(algebra_core)
endif()
