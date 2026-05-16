// *** tests/test_mapper_app.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/mappers/app.h>
#include <tiferet/assets/constants.h>
#include <string>

using namespace tiferet;

// *** tests: app_interface_aggregate

// ** test: app_interface_aggregate_mi_construction
TEST_CASE("AppInterfaceAggregate MI construction", "[mappers][app]") {
    AppInterfaceAggregate agg("basic_calc", "Basic Calculator");

    REQUIRE(agg.id == "basic_calc");
    REQUIRE(agg.name == "Basic Calculator");
    REQUIRE(agg.validate());
}

// ** test: app_interface_aggregate_set_attribute
TEST_CASE("AppInterfaceAggregate set_attribute updates fields", "[mappers][app]") {
    AppInterfaceAggregate agg("test", "Test");

    agg.set_attribute("name", "Updated");
    REQUIRE(agg.name == "Updated");

    agg.set_attribute("module_path", "tiferet.contexts.app");
    REQUIRE(agg.module_path == "tiferet.contexts.app");

    agg.set_attribute("logger_id", "custom");
    REQUIRE(agg.logger_id == "custom");
}

#if TIFERET_EXCEPTIONS

// ** test: app_interface_aggregate_set_attribute_unknown
TEST_CASE("AppInterfaceAggregate set_attribute raises for unknown", "[mappers][app]") {
    AppInterfaceAggregate agg("test", "Test");

    REQUIRE_THROWS_AS(
        agg.set_attribute("nonexistent", "value"),
        TiferetException
    );
}

#endif // TIFERET_EXCEPTIONS

// ** test: app_interface_aggregate_add_service
TEST_CASE("AppInterfaceAggregate add_service", "[mappers][app]") {
    AppInterfaceAggregate agg("test", "Test");

    agg.add_service(AppServiceDependency{"svc1", "mod.a", "ClassA"});
    agg.add_service(AppServiceDependency{"svc2", "mod.b", "ClassB"});

    REQUIRE(agg.services.size() == 2);
    REQUIRE(agg.services[0].service_id == "svc1");
    REQUIRE(agg.services[1].service_id == "svc2");
}

// ** test: app_interface_aggregate_remove_service
TEST_CASE("AppInterfaceAggregate remove_service", "[mappers][app]") {
    AppInterfaceAggregate agg("test", "Test", "", "", "", "default", {"default"}, {
        AppServiceDependency{"svc1", "mod.a", "ClassA"},
        AppServiceDependency{"svc2", "mod.b", "ClassB"}
    });

    bool removed = agg.remove_service("svc1");
    REQUIRE(removed);
    REQUIRE(agg.services.size() == 1);
    REQUIRE(agg.services[0].service_id == "svc2");

    // Idempotent: removing non-existent returns false.
    REQUIRE_FALSE(agg.remove_service("svc1"));
}

// ** test: app_interface_aggregate_set_constants
TEST_CASE("AppInterfaceAggregate set_constants merges", "[mappers][app]") {
    AppInterfaceAggregate agg("test", "Test", "", "", "", "default", {"default"}, {},
                              {{"key1", "val1"}});

    agg.set_constants({{"key2", "val2"}, {"key1", "updated"}});

    REQUIRE(agg.constants.at("key1") == "updated");
    REQUIRE(agg.constants.at("key2") == "val2");
}

// ** test: app_interface_aggregate_set_constants_remove
TEST_CASE("AppInterfaceAggregate set_constants removes empty values", "[mappers][app]") {
    AppInterfaceAggregate agg("test", "Test", "", "", "", "default", {"default"}, {},
                              {{"key1", "val1"}, {"key2", "val2"}});

    agg.set_constants({{"key1", ""}});

    REQUIRE(agg.constants.count("key1") == 0);
    REQUIRE(agg.constants.at("key2") == "val2");
}

// *** tests: app_interface_yaml_object

// ** test: app_yaml_to_map_default
TEST_CASE("AppInterfaceYamlObject to_map default", "[mappers][app]") {
    AppInterfaceYamlObject obj("basic_calc", "Basic Calculator",
                               "tiferet.contexts.app", "AppInterfaceContext");

    auto m = obj.to_map();
    REQUIRE(m.at("id") == "basic_calc");
    REQUIRE(m.at("name") == "Basic Calculator");
    REQUIRE(m.at("module_path") == "tiferet.contexts.app");
}

// ** test: app_yaml_to_map_to_data
TEST_CASE("AppInterfaceYamlObject to_map to_data excludes id", "[mappers][app]") {
    AppInterfaceYamlObject obj("test", "Test");

    auto m = obj.to_map("to_data");
    REQUIRE(m.count("id") == 0);
    REQUIRE(m.at("name") == "Test");
}

// ** test: app_yaml_round_trip
TEST_CASE("AppInterface round-trip: aggregate -> yaml -> aggregate", "[mappers][app]") {
    AppInterfaceAggregate original("calc_cli", "Calculator CLI",
                                   "tiferet.contexts.cli", "CliContext",
                                   "A CLI interface", "default", {"default"}, {
                                       AppServiceDependency{"cli_repo", "tiferet.proxies", "CliProxy",
                                                            {{"file", "cli.yml"}}}
                                   }, {{"version", "1.0"}});

    auto yaml_obj = AppInterfaceYamlObject::from_model(original);
    AppInterfaceAggregate round_tripped = yaml_obj.map();

    REQUIRE(round_tripped.id == original.id);
    REQUIRE(round_tripped.name == original.name);
    REQUIRE(round_tripped.module_path == original.module_path);
    REQUIRE(round_tripped.class_name == original.class_name);
    REQUIRE(round_tripped.description == original.description);
    REQUIRE(round_tripped.services.size() == original.services.size());
    REQUIRE(round_tripped.services[0].service_id == "cli_repo");
    REQUIRE(round_tripped.services[0].parameters.at("file") == "cli.yml");
    REQUIRE(round_tripped.constants.at("version") == "1.0");
}
