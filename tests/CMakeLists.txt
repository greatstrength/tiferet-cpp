# *** tests/CMakeLists.txt

# ** target: tiferet_tests
add_executable(tiferet_tests
    test_expected.cpp
    test_static_containers.cpp
    test_arena.cpp
    test_exceptions.cpp
    test_domain_object.cpp
    test_domain_event.cpp
    test_service.cpp
)

target_link_libraries(tiferet_tests PRIVATE tiferet Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(tiferet_tests)
