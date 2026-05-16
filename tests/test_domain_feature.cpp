// *** tests/test_domain_feature.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/domain/feature.h>
#include <string>

using namespace tiferet;

// *** tests: feature_step

// ** test: feature_step_construction
TEST_CASE("FeatureStep construction and validate", "[domain][feature]") {
    FeatureStep step("Add Numbers");

    REQUIRE(step.name == "Add Numbers");
    REQUIRE(step.type == "event");
    REQUIRE(step.validate());
}

// ** test: feature_step_validate_empty_name
TEST_CASE("FeatureStep validate fails on empty name", "[domain][feature]") {
    FeatureStep step("");

    REQUIRE_FALSE(step.validate());
}

// *** tests: feature_event

// ** test: feature_event_construction
TEST_CASE("FeatureEvent construction and validate", "[domain][feature]") {
    FeatureEvent event("Add a and b", "add_number_event",
                       {{"b", "0.5"}}, "result", false, "");

    REQUIRE(event.name == "Add a and b");
    REQUIRE(event.service_id == "add_number_event");
    REQUIRE(event.parameters.at("b") == "0.5");
    REQUIRE(event.data_key == "result");
    REQUIRE_FALSE(event.pass_on_error);
    REQUIRE(event.validate());
}

// ** test: feature_event_validate_requires_service_id
TEST_CASE("FeatureEvent validate requires service_id", "[domain][feature]") {
    FeatureEvent event("Step", "");

    REQUIRE_FALSE(event.validate());
}

// ** test: feature_event_default_values
TEST_CASE("FeatureEvent default values", "[domain][feature]") {
    FeatureEvent event("Step", "svc");

    REQUIRE(event.parameters.empty());
    REQUIRE(event.data_key.empty());
    REQUIRE_FALSE(event.pass_on_error);
    REQUIRE(event.condition.empty());
    REQUIRE(event.type == "event");
}

// *** tests: feature

// ** test: feature_construction
TEST_CASE("Feature construction and validate", "[domain][feature]") {
    Feature feat("calc.add", "Add Number", "calc", "add");

    REQUIRE(feat.id == "calc.add");
    REQUIRE(feat.name == "Add Number");
    REQUIRE(feat.group_id == "calc");
    REQUIRE(feat.feature_key == "add");
    REQUIRE(feat.validate());
}

// ** test: feature_validate_failure
TEST_CASE("Feature validate fails on missing fields", "[domain][feature]") {
    Feature feat("", "Add", "", "add");

    REQUIRE_FALSE(feat.validate());
}

// ** test: feature_get_step_valid
TEST_CASE("Feature get_step returns valid step", "[domain][feature]") {
    Feature feat("calc.add", "Add", "calc", "add", {
        FeatureEvent{"Step 1", "svc1"},
        FeatureEvent{"Step 2", "svc2"}
    });

    const auto* step = feat.get_step(0);
    REQUIRE(step != nullptr);
    REQUIRE(step->name == "Step 1");

    const auto* step2 = feat.get_step(1);
    REQUIRE(step2 != nullptr);
    REQUIRE(step2->name == "Step 2");
}

// ** test: feature_get_step_out_of_range
TEST_CASE("Feature get_step returns nullptr for invalid index", "[domain][feature]") {
    Feature feat("calc.add", "Add", "calc", "add");

    REQUIRE(feat.get_step(0) == nullptr);
    REQUIRE(feat.get_step(99) == nullptr);
}

// ** test: feature_derive_keys_from_dotted_id
TEST_CASE("Feature derive_keys splits dotted id", "[domain][feature]") {
    std::string id = "calc.add";
    std::string group_id, feature_key, name = "Add", desc;

    Feature::derive_keys(id, group_id, feature_key, name, desc);

    REQUIRE(group_id == "calc");
    REQUIRE(feature_key == "add");
    REQUIRE(desc == "Add");
}

// ** test: feature_derive_keys_from_parts
TEST_CASE("Feature derive_keys composes id from parts", "[domain][feature]") {
    std::string id;
    std::string group_id = "calc";
    std::string feature_key = "subtract";
    std::string name = "Subtract";
    std::string desc;

    Feature::derive_keys(id, group_id, feature_key, name, desc);

    REQUIRE(id == "calc.subtract");
    REQUIRE(desc == "Subtract");
}

// ** test: feature_derive_keys_from_name
TEST_CASE("Feature derive_keys generates feature_key from name", "[domain][feature]") {
    std::string id;
    std::string group_id = "calc";
    std::string feature_key;
    std::string name = "Add Number";
    std::string desc;

    Feature::derive_keys(id, group_id, feature_key, name, desc);

    REQUIRE(feature_key == "add_number");
    REQUIRE(id == "calc.add_number");
}
