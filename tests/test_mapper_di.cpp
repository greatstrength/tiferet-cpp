// *** tests/test_mapper_di.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/mappers/di.h>
#include <tiferet/assets/constants.h>
#include <string>

using namespace tiferet;

// *** tests: flagged_dependency_aggregate

// ** test: flagged_dependency_aggregate_construction
TEST_CASE("FlaggedDependencyAggregate MI construction", "[mappers][di]") {
    FlaggedDependencyAggregate agg("mod.yaml", "YamlImpl", "yaml");

    REQUIRE(agg.module_path == "mod.yaml");
    REQUIRE(agg.class_name == "YamlImpl");
    REQUIRE(agg.flag == "yaml");
    REQUIRE(agg.validate());
}

// ** test: flagged_dependency_aggregate_set_attribute
TEST_CASE("FlaggedDependencyAggregate set_attribute", "[mappers][di]") {
    FlaggedDependencyAggregate agg("mod", "Cls", "flag");

    agg.set_attribute("module_path", "new.mod");
    REQUIRE(agg.module_path == "new.mod");

    agg.set_attribute("class_name", "NewCls");
    REQUIRE(agg.class_name == "NewCls");
}

// ** test: flagged_dependency_aggregate_set_parameters
TEST_CASE("FlaggedDependencyAggregate set_parameters merges", "[mappers][di]") {
    FlaggedDependencyAggregate agg("mod", "Cls", "yaml",
                                   {{"file", "data.yml"}});

    agg.set_parameters({{"encoding", "utf-8"}, {"file", "updated.yml"}});

    REQUIRE(agg.parameters.at("file") == "updated.yml");
    REQUIRE(agg.parameters.at("encoding") == "utf-8");
}

// ** test: flagged_dependency_aggregate_set_parameters_remove
TEST_CASE("FlaggedDependencyAggregate set_parameters removes empty", "[mappers][di]") {
    FlaggedDependencyAggregate agg("mod", "Cls", "yaml",
                                   {{"file", "data.yml"}, {"extra", "val"}});

    agg.set_parameters({{"file", ""}});

    REQUIRE(agg.parameters.count("file") == 0);
    REQUIRE(agg.parameters.at("extra") == "val");
}

// *** tests: service_configuration_aggregate

// ** test: svc_config_aggregate_construction
TEST_CASE("ServiceConfigurationAggregate MI construction", "[mappers][di]") {
    ServiceConfigurationAggregate agg("error_service", "Error Service",
                                      "mod.error", "ErrorRepo");

    REQUIRE(agg.id == "error_service");
    REQUIRE(agg.name == "Error Service");
    REQUIRE(agg.validate());
}

// ** test: svc_config_aggregate_set_default_type
TEST_CASE("ServiceConfigurationAggregate set_default_type", "[mappers][di]") {
    ServiceConfigurationAggregate agg("svc");

    agg.set_default_type("mod.new", "NewCls", {{"key", "val"}});

    REQUIRE(agg.module_path == "mod.new");
    REQUIRE(agg.class_name == "NewCls");
    REQUIRE(agg.parameters.at("key") == "val");
}

// ** test: svc_config_aggregate_set_dependency_new
TEST_CASE("ServiceConfigurationAggregate set_dependency adds new", "[mappers][di]") {
    ServiceConfigurationAggregate agg("svc");

    agg.set_dependency("yaml", "mod.yaml", "YamlImpl", {{"file", "data.yml"}});

    REQUIRE(agg.dependencies.size() == 1);
    REQUIRE(agg.dependencies[0].flag == "yaml");
    REQUIRE(agg.dependencies[0].module_path == "mod.yaml");
    REQUIRE(agg.dependencies[0].parameters.at("file") == "data.yml");
}

// ** test: svc_config_aggregate_set_dependency_update
TEST_CASE("ServiceConfigurationAggregate set_dependency updates existing", "[mappers][di]") {
    ServiceConfigurationAggregate agg("svc", "", "", "", {}, {
        FlaggedDependency{"mod.yaml", "YamlImpl", "yaml", {{"file", "old.yml"}}}
    });

    agg.set_dependency("yaml", "mod.yaml.v2", "YamlImplV2", {{"file", "new.yml"}});

    REQUIRE(agg.dependencies.size() == 1);
    REQUIRE(agg.dependencies[0].module_path == "mod.yaml.v2");
    REQUIRE(agg.dependencies[0].class_name == "YamlImplV2");
    REQUIRE(agg.dependencies[0].parameters.at("file") == "new.yml");
}

// ** test: svc_config_aggregate_remove_dependency
TEST_CASE("ServiceConfigurationAggregate remove_dependency", "[mappers][di]") {
    ServiceConfigurationAggregate agg("svc", "", "", "", {}, {
        FlaggedDependency{"mod.yaml", "YamlImpl", "yaml"},
        FlaggedDependency{"mod.json", "JsonImpl", "json"}
    });

    agg.remove_dependency("yaml");

    REQUIRE(agg.dependencies.size() == 1);
    REQUIRE(agg.dependencies[0].flag == "json");
}

#if TIFERET_EXCEPTIONS

// ** test: svc_config_aggregate_set_attribute_unknown
TEST_CASE("ServiceConfigurationAggregate set_attribute raises for unknown", "[mappers][di]") {
    ServiceConfigurationAggregate agg("svc");

    REQUIRE_THROWS_AS(
        agg.set_attribute("nonexistent", "value"),
        TiferetException
    );
}

#endif // TIFERET_EXCEPTIONS

// *** tests: service_configuration_yaml_object

// ** test: svc_config_yaml_to_map
TEST_CASE("ServiceConfigurationYamlObject to_map", "[mappers][di]") {
    ServiceConfigurationYamlObject obj("error_service", "Error Service",
                                       "mod.error", "ErrorRepo");

    auto m = obj.to_map();
    REQUIRE(m.at("id") == "error_service");
    REQUIRE(m.at("name") == "Error Service");
    REQUIRE(m.at("module_path") == "mod.error");
}

// ** test: svc_config_yaml_to_map_to_data
TEST_CASE("ServiceConfigurationYamlObject to_map to_data excludes id", "[mappers][di]") {
    ServiceConfigurationYamlObject obj("svc", "Svc");

    auto m = obj.to_map("to_data");
    REQUIRE(m.count("id") == 0);
    REQUIRE(m.at("name") == "Svc");
}

// ** test: svc_config_yaml_round_trip
TEST_CASE("ServiceConfiguration round-trip: aggregate -> yaml -> aggregate", "[mappers][di]") {
    ServiceConfigurationAggregate original("error_service", "Error Service",
                                           "mod.error", "ErrorRepo",
                                           {{"file", "error.yml"}}, {
                                               FlaggedDependency{"mod.yaml", "YamlImpl", "yaml",
                                                                 {{"encoding", "utf-8"}}},
                                               FlaggedDependency{"mod.json", "JsonImpl", "json"}
                                           });

    auto yaml_obj = ServiceConfigurationYamlObject::from_model(original);
    ServiceConfigurationAggregate round_tripped = yaml_obj.map();

    REQUIRE(round_tripped.id == original.id);
    REQUIRE(round_tripped.name == original.name);
    REQUIRE(round_tripped.module_path == original.module_path);
    REQUIRE(round_tripped.class_name == original.class_name);
    REQUIRE(round_tripped.parameters.at("file") == "error.yml");
    REQUIRE(round_tripped.dependencies.size() == 2);
    REQUIRE(round_tripped.dependencies[0].flag == "yaml");
    REQUIRE(round_tripped.dependencies[0].parameters.at("encoding") == "utf-8");
    REQUIRE(round_tripped.dependencies[1].flag == "json");
}

// ** test: flagged_dependency_yaml_round_trip
TEST_CASE("FlaggedDependency round-trip: model -> yaml -> model", "[mappers][di]") {
    FlaggedDependency original("mod.yaml", "YamlImpl", "yaml",
                               {{"file", "data.yml"}});

    auto yaml_obj = FlaggedDependencyYamlObject::from_model(original);
    FlaggedDependency round_tripped = yaml_obj.map();

    REQUIRE(round_tripped.module_path == original.module_path);
    REQUIRE(round_tripped.class_name == original.class_name);
    REQUIRE(round_tripped.flag == original.flag);
    REQUIRE(round_tripped.parameters.at("file") == "data.yml");
}
