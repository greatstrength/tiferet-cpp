// *** tests/test_domain_di.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/domain/di.h>
#include <string>

using namespace tiferet;

// *** tests: flagged_dependency

// ** test: flagged_dependency_construction
TEST_CASE("FlaggedDependency construction and validate", "[domain][di]") {
    FlaggedDependency dep("tiferet.repos.error", "ErrorYamlRepository", "yaml",
                          {{"error_yaml_file", "app/configs/error.yml"}});

    REQUIRE(dep.module_path == "tiferet.repos.error");
    REQUIRE(dep.class_name == "ErrorYamlRepository");
    REQUIRE(dep.flag == "yaml");
    REQUIRE(dep.parameters.at("error_yaml_file") == "app/configs/error.yml");
    REQUIRE(dep.validate());
}

// ** test: flagged_dependency_validate_failure
TEST_CASE("FlaggedDependency validate fails on empty fields", "[domain][di]") {
    FlaggedDependency dep("", "Cls", "flag");
    REQUIRE_FALSE(dep.validate());

    FlaggedDependency dep2("mod", "Cls", "");
    REQUIRE_FALSE(dep2.validate());
}

// *** tests: service_configuration

// ** test: service_configuration_construction
TEST_CASE("ServiceConfiguration construction and validate", "[domain][di]") {
    ServiceConfiguration cfg("error_service", "Error Service",
                             "tiferet.repos.error", "ErrorYamlRepository");

    REQUIRE(cfg.id == "error_service");
    REQUIRE(cfg.name == "Error Service");
    REQUIRE(cfg.module_path == "tiferet.repos.error");
    REQUIRE(cfg.class_name == "ErrorYamlRepository");
    REQUIRE(cfg.validate());
}

// ** test: service_configuration_minimal
TEST_CASE("ServiceConfiguration validates with just id", "[domain][di]") {
    ServiceConfiguration cfg("svc");
    REQUIRE(cfg.validate());
}

// ** test: service_configuration_validate_failure
TEST_CASE("ServiceConfiguration validate fails on empty id", "[domain][di]") {
    ServiceConfiguration cfg("");
    REQUIRE_FALSE(cfg.validate());
}

// ** test: service_configuration_get_dependency_found
TEST_CASE("ServiceConfiguration get_dependency finds by flag", "[domain][di]") {
    ServiceConfiguration cfg("svc", "", "", "", {}, {
        FlaggedDependency{"mod.yaml", "YamlImpl", "yaml"},
        FlaggedDependency{"mod.json", "JsonImpl", "json"}
    });

    const auto* dep = cfg.get_dependency("json");
    REQUIRE(dep != nullptr);
    REQUIRE(dep->class_name == "JsonImpl");
}

// ** test: service_configuration_get_dependency_not_found
TEST_CASE("ServiceConfiguration get_dependency returns nullptr for unknown flag", "[domain][di]") {
    ServiceConfiguration cfg("svc", "", "", "", {}, {
        FlaggedDependency{"mod", "Cls", "yaml"}
    });

    REQUIRE(cfg.get_dependency("sqlite") == nullptr);
}

// ** test: service_configuration_get_dependency_priority
TEST_CASE("ServiceConfiguration get_dependency respects flag priority", "[domain][di]") {
    ServiceConfiguration cfg("svc", "", "", "", {}, {
        FlaggedDependency{"mod.yaml", "YamlImpl", "yaml"},
        FlaggedDependency{"mod.json", "JsonImpl", "json"}
    });

    // "json" is checked first, should match even though "yaml" also exists.
    const auto* dep = cfg.get_dependency("json", "yaml");
    REQUIRE(dep != nullptr);
    REQUIRE(dep->flag == "json");
}

// ** test: service_configuration_with_parameters
TEST_CASE("ServiceConfiguration stores default parameters", "[domain][di]") {
    ServiceConfiguration cfg("svc", "", "mod", "Cls",
                             {{"config_file", "app.yml"}, {"encoding", "utf-8"}});

    REQUIRE(cfg.parameters.at("config_file") == "app.yml");
    REQUIRE(cfg.parameters.at("encoding") == "utf-8");
}
