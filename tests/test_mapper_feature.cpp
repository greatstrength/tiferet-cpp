// *** tests/test_mapper_feature.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/mappers/feature.h>
#include <tiferet/assets/constants.h>
#include <string>

using namespace tiferet;

// *** tests: feature_event_aggregate

// ** test: feature_event_aggregate_mi_construction
TEST_CASE("FeatureEventAggregate MI construction", "[mappers][feature]") {
    FeatureEventAggregate agg("Add a and b", "add_number_event",
                              {{"b", "0.5"}});

    REQUIRE(agg.name == "Add a and b");
    REQUIRE(agg.service_id == "add_number_event");
    REQUIRE(agg.parameters.at("b") == "0.5");
    REQUIRE(agg.type == "event");
    REQUIRE(agg.validate());
}

// ** test: feature_event_aggregate_set_attribute
TEST_CASE("FeatureEventAggregate set_attribute updates fields", "[mappers][feature]") {
    FeatureEventAggregate agg("Step", "svc");

    agg.set_attribute("name", "New Step");
    REQUIRE(agg.name == "New Step");

    agg.set_attribute("service_id", "new_svc");
    REQUIRE(agg.service_id == "new_svc");

    agg.set_attribute("data_key", "result");
    REQUIRE(agg.data_key == "result");

    agg.set_attribute("condition", "x > 0");
    REQUIRE(agg.condition == "x > 0");
}

// ** test: feature_event_aggregate_set_pass_on_error
TEST_CASE("FeatureEventAggregate set_attribute normalizes pass_on_error", "[mappers][feature]") {
    FeatureEventAggregate agg("Step", "svc");

    agg.set_attribute("pass_on_error", "true");
    REQUIRE(agg.pass_on_error == true);

    agg.set_attribute("pass_on_error", "false");
    REQUIRE(agg.pass_on_error == false);

    agg.set_attribute("pass_on_error", "False");
    REQUIRE(agg.pass_on_error == false);

    agg.set_attribute("pass_on_error", "1");
    REQUIRE(agg.pass_on_error == true);

    agg.set_attribute("pass_on_error", "0");
    REQUIRE(agg.pass_on_error == false);
}

#if TIFERET_EXCEPTIONS

// ** test: feature_event_aggregate_set_attribute_unknown
TEST_CASE("FeatureEventAggregate set_attribute raises for unknown field", "[mappers][feature]") {
    FeatureEventAggregate agg("Step", "svc");

    REQUIRE_THROWS_AS(
        agg.set_attribute("nonexistent", "value"),
        TiferetException
    );
}

#endif // TIFERET_EXCEPTIONS

// *** tests: feature_event_yaml_object

// ** test: feature_event_yaml_to_map
TEST_CASE("FeatureEventYamlObject to_map", "[mappers][feature]") {
    FeatureEventYamlObject obj("Add a and b", "add_event",
                               {}, "result", true);

    auto m = obj.to_map();

    REQUIRE(m.at("name") == "Add a and b");
    REQUIRE(m.at("service_id") == "add_event");
    REQUIRE(m.at("data_key") == "result");
    REQUIRE(m.at("pass_on_error") == "true");
}

// ** test: feature_event_yaml_map
TEST_CASE("FeatureEventYamlObject map to aggregate", "[mappers][feature]") {
    FeatureEventYamlObject obj("Step", "svc", {{"a", "1"}});

    FeatureEventAggregate agg = obj.map();

    REQUIRE(agg.name == "Step");
    REQUIRE(agg.service_id == "svc");
    REQUIRE(agg.parameters.at("a") == "1");
}

// ** test: feature_event_yaml_from_model
TEST_CASE("FeatureEventYamlObject from_model", "[mappers][feature]") {
    FeatureEvent event("Step", "svc", {{"x", "2"}}, "out", true, "y > 0");

    auto obj = FeatureEventYamlObject::from_model(event);

    REQUIRE(obj.name == "Step");
    REQUIRE(obj.service_id == "svc");
    REQUIRE(obj.parameters.at("x") == "2");
    REQUIRE(obj.data_key == "out");
    REQUIRE(obj.pass_on_error == true);
    REQUIRE(obj.condition == "y > 0");
}

// *** tests: feature_aggregate

// ** test: feature_aggregate_mi_construction
TEST_CASE("FeatureAggregate MI construction", "[mappers][feature]") {
    FeatureAggregate agg("calc.add", "Add Number", "calc", "add");

    REQUIRE(agg.id == "calc.add");
    REQUIRE(agg.name == "Add Number");
    REQUIRE(agg.group_id == "calc");
    REQUIRE(agg.feature_key == "add");
    REQUIRE(agg.validate());
}

// ** test: feature_aggregate_set_attribute
TEST_CASE("FeatureAggregate set_attribute updates fields", "[mappers][feature]") {
    FeatureAggregate agg("calc.add", "Add", "calc", "add");

    agg.set_attribute("name", "Addition");
    REQUIRE(agg.name == "Addition");

    agg.set_attribute("description", "Adds two numbers");
    REQUIRE(agg.description == "Adds two numbers");
}

#if TIFERET_EXCEPTIONS

// ** test: feature_aggregate_set_attribute_unknown
TEST_CASE("FeatureAggregate set_attribute raises for unknown field", "[mappers][feature]") {
    FeatureAggregate agg("calc.add", "Add", "calc", "add");

    REQUIRE_THROWS_AS(
        agg.set_attribute("nonexistent", "value"),
        TiferetException
    );
}

#endif // TIFERET_EXCEPTIONS

// ** test: feature_aggregate_rename
TEST_CASE("FeatureAggregate rename", "[mappers][feature]") {
    FeatureAggregate agg("calc.add", "Original", "calc", "add");

    agg.rename("Renamed");

    REQUIRE(agg.name == "Renamed");
}

// ** test: feature_aggregate_add_step
TEST_CASE("FeatureAggregate add_step appends", "[mappers][feature]") {
    FeatureAggregate agg("calc.add", "Add", "calc", "add");

    agg.add_step(FeatureEvent{"Step 1", "svc1"});
    agg.add_step(FeatureEvent{"Step 2", "svc2"});

    REQUIRE(agg.steps.size() == 2);
    REQUIRE(agg.steps[0].name == "Step 1");
    REQUIRE(agg.steps[1].name == "Step 2");
}

// ** test: feature_aggregate_add_step_at_position
TEST_CASE("FeatureAggregate add_step at position", "[mappers][feature]") {
    FeatureAggregate agg("calc.add", "Add", "calc", "add", {
        FeatureEvent{"Step 1", "svc1"},
        FeatureEvent{"Step 3", "svc3"}
    });

    agg.add_step(FeatureEvent{"Step 2", "svc2"}, 1);

    REQUIRE(agg.steps.size() == 3);
    REQUIRE(agg.steps[0].name == "Step 1");
    REQUIRE(agg.steps[1].name == "Step 2");
    REQUIRE(agg.steps[2].name == "Step 3");
}

// *** tests: feature_yaml_object

// ** test: feature_yaml_to_map_default
TEST_CASE("FeatureYamlObject to_map default role", "[mappers][feature]") {
    FeatureYamlObject obj("calc.add", "Add Number", "calc", "add",
                          {}, "Adds two numbers");

    auto m = obj.to_map();

    REQUIRE(m.at("id") == "calc.add");
    REQUIRE(m.at("name") == "Add Number");
    REQUIRE(m.at("group_id") == "calc");
    REQUIRE(m.at("feature_key") == "add");
    REQUIRE(m.at("description") == "Adds two numbers");
}

// ** test: feature_yaml_to_map_to_data
TEST_CASE("FeatureYamlObject to_map to_data excludes derived keys", "[mappers][feature]") {
    FeatureYamlObject obj("calc.add", "Add", "calc", "add",
                          {}, "Adds numbers");

    auto m = obj.to_map("to_data");

    REQUIRE(m.count("id") == 0);
    REQUIRE(m.count("group_id") == 0);
    REQUIRE(m.count("feature_key") == 0);
    REQUIRE(m.at("name") == "Add");
    REQUIRE(m.at("description") == "Adds numbers");
}

// ** test: feature_yaml_map_to_aggregate
TEST_CASE("FeatureYamlObject map produces FeatureAggregate", "[mappers][feature]") {
    FeatureYamlObject obj("calc.add", "Add", "calc", "add", {
        FeatureEvent{"Step 1", "svc1"}
    }, "Addition");

    FeatureAggregate agg = obj.map();

    REQUIRE(agg.id == "calc.add");
    REQUIRE(agg.name == "Add");
    REQUIRE(agg.group_id == "calc");
    REQUIRE(agg.feature_key == "add");
    REQUIRE(agg.description == "Addition");
    REQUIRE(agg.steps.size() == 1);
    REQUIRE(agg.steps[0].name == "Step 1");
}

// ** test: feature_yaml_from_model
TEST_CASE("FeatureYamlObject from_model", "[mappers][feature]") {
    Feature feat("calc.subtract", "Subtract", "calc", "subtract", {
        FeatureEvent{"Sub step", "sub_svc"}
    }, "Subtraction");

    auto obj = FeatureYamlObject::from_model(feat);

    REQUIRE(obj.id == "calc.subtract");
    REQUIRE(obj.name == "Subtract");
    REQUIRE(obj.steps.size() == 1);
    REQUIRE(obj.steps[0].service_id == "sub_svc");
}

// ** test: feature_round_trip
TEST_CASE("Feature round-trip: aggregate -> yaml -> aggregate", "[mappers][feature]") {
    // Start with an aggregate.
    FeatureAggregate original("calc.add", "Add Number", "calc", "add", {
        FeatureEvent{"Add a and b", "add_event", {{"b", "0.5"}}, "result", false, ""},
        FeatureEvent{"Validate", "validate_event", {}, "", true, "x > 0"}
    }, "Addition feature");

    // Convert to transfer object.
    auto yaml_obj = FeatureYamlObject::from_model(original);

    // Convert back to aggregate.
    FeatureAggregate round_tripped = yaml_obj.map();

    // Verify scalar fields survived the round-trip.
    REQUIRE(round_tripped.id == original.id);
    REQUIRE(round_tripped.name == original.name);
    REQUIRE(round_tripped.group_id == original.group_id);
    REQUIRE(round_tripped.feature_key == original.feature_key);
    REQUIRE(round_tripped.description == original.description);

    // Verify steps survived the round-trip.
    REQUIRE(round_tripped.steps.size() == original.steps.size());
    REQUIRE(round_tripped.steps[0].name == original.steps[0].name);
    REQUIRE(round_tripped.steps[0].service_id == original.steps[0].service_id);
    REQUIRE(round_tripped.steps[0].parameters.at("b") == "0.5");
    REQUIRE(round_tripped.steps[1].name == original.steps[1].name);
    REQUIRE(round_tripped.steps[1].pass_on_error == true);
    REQUIRE(round_tripped.steps[1].condition == "x > 0");
}
