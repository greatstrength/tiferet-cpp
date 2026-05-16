// *** tests/test_mapper_error.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/mappers/error.h>
#include <tiferet/assets/constants.h>
#include <string>

using namespace tiferet;

// *** tests: error_aggregate

// ** test: error_aggregate_mi_construction
TEST_CASE("ErrorAggregate MI construction", "[mappers][error]") {
    ErrorAggregate agg("invalid_input", "Invalid Input", {
        ErrorMessage{"en_US", "Value must be a number"}
    });

    REQUIRE(agg.id == "invalid_input");
    REQUIRE(agg.name == "Invalid Input");
    REQUIRE(agg.error_code == "INVALID_INPUT");
    REQUIRE(agg.message.size() == 1);
    REQUIRE(agg.validate());
}

// ** test: error_aggregate_set_attribute_name
TEST_CASE("ErrorAggregate set_attribute updates name", "[mappers][error]") {
    ErrorAggregate agg("test", "Original");

    agg.set_attribute("name", "Updated");

    REQUIRE(agg.name == "Updated");
}

// ** test: error_aggregate_set_attribute_description
TEST_CASE("ErrorAggregate set_attribute updates description", "[mappers][error]") {
    ErrorAggregate agg("test", "Test");

    agg.set_attribute("description", "A test error");

    REQUIRE(agg.description == "A test error");
}

#if TIFERET_EXCEPTIONS

// ** test: error_aggregate_set_attribute_unknown
TEST_CASE("ErrorAggregate set_attribute raises for unknown field", "[mappers][error]") {
    ErrorAggregate agg("test", "Test");

    REQUIRE_THROWS_AS(
        agg.set_attribute("nonexistent", "value"),
        TiferetException
    );
}

#endif // TIFERET_EXCEPTIONS

// ** test: error_aggregate_rename
TEST_CASE("ErrorAggregate rename", "[mappers][error]") {
    ErrorAggregate agg("test", "Original");

    agg.rename("Renamed");

    REQUIRE(agg.name == "Renamed");
}

// ** test: error_aggregate_set_message_new
TEST_CASE("ErrorAggregate set_message adds new language", "[mappers][error]") {
    ErrorAggregate agg("test", "Test");

    agg.set_message("en_US", "English error");

    REQUIRE(agg.message.size() == 1);
    REQUIRE(agg.message[0].lang == "en_US");
    REQUIRE(agg.message[0].text == "English error");
}

// ** test: error_aggregate_set_message_update
TEST_CASE("ErrorAggregate set_message updates existing language", "[mappers][error]") {
    ErrorAggregate agg("test", "Test", {
        ErrorMessage{"en_US", "Old text"}
    });

    agg.set_message("en_US", "New text");

    REQUIRE(agg.message.size() == 1);
    REQUIRE(agg.message[0].text == "New text");
}

// ** test: error_aggregate_remove_message
TEST_CASE("ErrorAggregate remove_message", "[mappers][error]") {
    ErrorAggregate agg("test", "Test", {
        ErrorMessage{"en_US", "English"},
        ErrorMessage{"es_ES", "Spanish"}
    });

    agg.remove_message("en_US");

    REQUIRE(agg.message.size() == 1);
    REQUIRE(agg.message[0].lang == "es_ES");
}

// *** tests: error_message_yaml_object

// ** test: error_message_yaml_to_map
TEST_CASE("ErrorMessageYamlObject to_map", "[mappers][error]") {
    ErrorMessageYamlObject obj("en_US", "Test message");

    auto m = obj.to_map();

    REQUIRE(m.at("lang") == "en_US");
    REQUIRE(m.at("text") == "Test message");
}

// ** test: error_message_yaml_map
TEST_CASE("ErrorMessageYamlObject map to ErrorMessage", "[mappers][error]") {
    ErrorMessageYamlObject obj("en_US", "Test message");

    ErrorMessage msg = obj.map();

    REQUIRE(msg.lang == "en_US");
    REQUIRE(msg.text == "Test message");
}

// ** test: error_message_yaml_from_model
TEST_CASE("ErrorMessageYamlObject from_model", "[mappers][error]") {
    ErrorMessage msg("es_ES", "Mensaje de prueba");

    auto obj = ErrorMessageYamlObject::from_model(msg);

    REQUIRE(obj.lang == "es_ES");
    REQUIRE(obj.text == "Mensaje de prueba");
}

// *** tests: error_yaml_object

// ** test: error_yaml_to_map_default
TEST_CASE("ErrorYamlObject to_map default role", "[mappers][error]") {
    ErrorYamlObject obj("invalid_input", "Invalid Input");

    auto m = obj.to_map();

    REQUIRE(m.at("id") == "invalid_input");
    REQUIRE(m.at("name") == "Invalid Input");
    REQUIRE(m.at("error_code") == "INVALID_INPUT");
}

// ** test: error_yaml_to_map_to_model
TEST_CASE("ErrorYamlObject to_map to_model role", "[mappers][error]") {
    ErrorYamlObject obj("test", "Test");

    auto m = obj.to_map("to_model");

    REQUIRE(m.count("id") == 1);
    REQUIRE(m.count("name") == 1);
}

// ** test: error_yaml_to_map_to_data
TEST_CASE("ErrorYamlObject to_map to_data role excludes id", "[mappers][error]") {
    ErrorYamlObject obj("test", "Test");

    auto m = obj.to_map("to_data");

    REQUIRE(m.count("id") == 0);
    REQUIRE(m.count("name") == 1);
}

// ** test: error_yaml_map_to_aggregate
TEST_CASE("ErrorYamlObject map produces ErrorAggregate", "[mappers][error]") {
    ErrorYamlObject obj("invalid_input", "Invalid Input", {
        ErrorMessage{"en_US", "Value must be a number"}
    }, "An invalid input error");

    ErrorAggregate agg = obj.map();

    REQUIRE(agg.id == "invalid_input");
    REQUIRE(agg.name == "Invalid Input");
    REQUIRE(agg.error_code == "INVALID_INPUT");
    REQUIRE(agg.description == "An invalid input error");
    REQUIRE(agg.message.size() == 1);
    REQUIRE(agg.message[0].text == "Value must be a number");
}

// ** test: error_yaml_from_model
TEST_CASE("ErrorYamlObject from_model", "[mappers][error]") {
    Error err("division_by_zero", "Division By Zero", {
        ErrorMessage{"en_US", "Cannot divide by zero"}
    });

    auto obj = ErrorYamlObject::from_model(err);

    REQUIRE(obj.id == "division_by_zero");
    REQUIRE(obj.name == "Division By Zero");
    REQUIRE(obj.error_code == "DIVISION_BY_ZERO");
    REQUIRE(obj.message.size() == 1);
}

// ** test: error_round_trip
TEST_CASE("Error round-trip: aggregate -> yaml -> aggregate", "[mappers][error]") {
    // Start with an aggregate.
    ErrorAggregate original("invalid_input", "Invalid Input", {
        ErrorMessage{"en_US", "Value must be a number"},
        ErrorMessage{"es_ES", "El valor debe ser un número"}
    }, "Input validation error");

    // Convert to transfer object.
    auto yaml_obj = ErrorYamlObject::from_model(original);

    // Convert back to aggregate.
    ErrorAggregate round_tripped = yaml_obj.map();

    // Verify all fields survived the round-trip.
    REQUIRE(round_tripped.id == original.id);
    REQUIRE(round_tripped.name == original.name);
    REQUIRE(round_tripped.error_code == original.error_code);
    REQUIRE(round_tripped.description == original.description);
    REQUIRE(round_tripped.message.size() == original.message.size());
    REQUIRE(round_tripped.message[0].lang == original.message[0].lang);
    REQUIRE(round_tripped.message[0].text == original.message[0].text);
    REQUIRE(round_tripped.message[1].lang == original.message[1].lang);
    REQUIRE(round_tripped.message[1].text == original.message[1].text);
}
