// *** tests/test_domain_app.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/domain/app.h>
#include <string>

using namespace tiferet;

// *** tests: app_service_dependency

// ** test: app_service_dependency_construction
TEST_CASE("AppServiceDependency construction and validate", "[domain][app]") {
    AppServiceDependency dep("error_repo", "tiferet.repos.error", "ErrorYamlRepository",
                             {{"error_yaml_file", "app/configs/error.yml"}});

    REQUIRE(dep.service_id == "error_repo");
    REQUIRE(dep.module_path == "tiferet.repos.error");
    REQUIRE(dep.class_name == "ErrorYamlRepository");
    REQUIRE(dep.parameters.at("error_yaml_file") == "app/configs/error.yml");
    REQUIRE(dep.validate());
}

// ** test: app_service_dependency_validate_failure
TEST_CASE("AppServiceDependency validate fails on empty fields", "[domain][app]") {
    AppServiceDependency dep("", "mod", "cls");
    REQUIRE_FALSE(dep.validate());

    AppServiceDependency dep2("svc", "", "cls");
    REQUIRE_FALSE(dep2.validate());
}

// *** tests: app_interface

// ** test: app_interface_construction
TEST_CASE("AppInterface construction and validate", "[domain][app]") {
    AppInterface iface("basic_calc", "Basic Calculator");

    REQUIRE(iface.id == "basic_calc");
    REQUIRE(iface.name == "Basic Calculator");
    REQUIRE(iface.logger_id == "default");
    REQUIRE(iface.flags.size() == 1);
    REQUIRE(iface.flags[0] == "default");
    REQUIRE(iface.validate());
}

// ** test: app_interface_with_services
TEST_CASE("AppInterface with services", "[domain][app]") {
    AppInterface iface("calc_cli", "Calculator CLI",
                       "tiferet.contexts.cli", "CliContext", "CLI interface", "default",
                       {"default"}, {
                           AppServiceDependency{"cli_repo", "tiferet.proxies.yaml.cli", "CliYamlProxy",
                                                {{"cli_config_file", "app/configs/cli.yml"}}},
                           AppServiceDependency{"cli_service", "tiferet.handlers.cli", "CliHandler"}
                       });

    REQUIRE(iface.services.size() == 2);
    REQUIRE(iface.module_path == "tiferet.contexts.cli");
    REQUIRE(iface.class_name == "CliContext");
}

// ** test: app_interface_get_service_found
TEST_CASE("AppInterface get_service returns matching dependency", "[domain][app]") {
    AppInterface iface("test", "Test", "", "", "", "default", {"default"}, {
        AppServiceDependency{"svc_a", "mod.a", "ClassA"},
        AppServiceDependency{"svc_b", "mod.b", "ClassB"}
    });

    const auto* dep = iface.get_service("svc_b");
    REQUIRE(dep != nullptr);
    REQUIRE(dep->class_name == "ClassB");
}

// ** test: app_interface_get_service_not_found
TEST_CASE("AppInterface get_service returns nullptr for unknown id", "[domain][app]") {
    AppInterface iface("test", "Test");

    REQUIRE(iface.get_service("nonexistent") == nullptr);
}

// ** test: app_interface_validate_failure
TEST_CASE("AppInterface validate fails on empty id", "[domain][app]") {
    AppInterface iface("", "Test");
    REQUIRE_FALSE(iface.validate());
}

// ** test: app_interface_constants
TEST_CASE("AppInterface stores constants", "[domain][app]") {
    AppInterface iface("test", "Test", "", "", "", "default", {"default"}, {},
                       {{"version", "1.0"}, {"env", "production"}});

    REQUIRE(iface.constants.at("version") == "1.0");
    REQUIRE(iface.constants.at("env") == "production");
}
