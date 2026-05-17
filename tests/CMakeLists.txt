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
    test_domain_error.cpp
    test_domain_feature.cpp
    test_mapper_settings.cpp
    test_mapper_error.cpp
    test_mapper_feature.cpp
    test_yaml.cpp
    test_domain_app.cpp
    test_domain_di.cpp
    test_mapper_app.cpp
    test_mapper_di.cpp
    test_registry.cpp
    test_di_container.cpp
    test_feature_context.cpp
    test_repo_feature.cpp
    test_repo_error.cpp
    test_repo_di.cpp
    test_repo_app.cpp
    test_integration_calc.cpp
)

target_link_libraries(tiferet_tests PRIVATE tiferet Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(tiferet_tests)
