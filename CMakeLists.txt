cmake_minimum_required(VERSION 3.16)
project(tiferet
    VERSION 1.0.0
    DESCRIPTION "A C++ framework for Domain-Driven Design in firmware and embedded systems"
    LANGUAGES CXX
)

# *** options

# ** option: C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ** option: allocation mode
set(TIFERET_ALLOC_MODE "HEAP" CACHE STRING "Allocation mode: HEAP, ARENA, or STATIC")
set_property(CACHE TIFERET_ALLOC_MODE PROPERTY STRINGS HEAP ARENA STATIC)

# ** option: exception mode
option(TIFERET_EXCEPTIONS "Enable C++ exceptions" ON)

# ** option: build tests
option(TIFERET_BUILD_TESTS "Build unit tests" ON)

# *** library

# ** target: tiferet (header-only interface library)
add_library(tiferet INTERFACE)
target_include_directories(tiferet INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# ** compile definitions: allocation mode
target_compile_definitions(tiferet INTERFACE
    TIFERET_ALLOC_${TIFERET_ALLOC_MODE}=1
)

# ** compile definitions: exception mode
if(TIFERET_EXCEPTIONS)
    target_compile_definitions(tiferet INTERFACE TIFERET_EXCEPTIONS=1)
else()
    target_compile_definitions(tiferet INTERFACE TIFERET_EXCEPTIONS=0)
    target_compile_options(tiferet INTERFACE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-fno-exceptions>
    )
endif()

# *** tests

if(TIFERET_BUILD_TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
    )
    FetchContent_MakeAvailable(Catch2)

    add_subdirectory(tests)
endif()
