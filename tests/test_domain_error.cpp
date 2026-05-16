// *** tests/test_domain_error.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/domain/error.h>
#include <string>

using namespace tiferet;

// *** tests: error_message

// ** test: error_message_construction
TEST_CASE("ErrorMessage construction and validate", "[domain][error]") {
    ErrorMessage msg("en_US", "Value must be a number");

    REQUIRE(msg.lang == "en_US");
    REQUIRE(msg.text == "Value must be a number");
    REQUIRE(msg.validate());
}

// ** test: error_message_validate_empty_lang
TEST_CASE("ErrorMessage validate fails on empty lang", "[domain][error]") {
    ErrorMessage msg("", "some text");

    REQUIRE_FALSE(msg.validate());
}

// ** test: error_message_validate_empty_text
TEST_CASE("ErrorMessage validate fails on empty text", "[domain][error]") {
    ErrorMessage msg("en_US", "");

    REQUIRE_FALSE(msg.validate());
}

// ** test: error_message_format
TEST_CASE("ErrorMessage format returns text", "[domain][error]") {
    ErrorMessage msg("en_US", "Cannot divide by zero");

    REQUIRE(msg.format() == "Cannot divide by zero");
}

// *** tests: error

// ** test: error_derived_error_code
TEST_CASE("Error derives error_code from id", "[domain][error]") {
    Error err("invalid_input", "Invalid Input");

    REQUIRE(err.id == "invalid_input");
    REQUIRE(err.name == "Invalid Input");
    REQUIRE(err.error_code == "INVALID_INPUT");
}

// ** test: error_explicit_error_code
TEST_CASE("Error uses explicit error_code when provided", "[domain][error]") {
    Error err("my_error", "My Error", {}, "", "CUSTOM_CODE");

    REQUIRE(err.error_code == "CUSTOM_CODE");
}

// ** test: error_derive_with_spaces
TEST_CASE("Error derive_error_code converts spaces to underscores", "[domain][error]") {
    std::string code = Error::derive_error_code("not found error");

    REQUIRE(code == "NOT_FOUND_ERROR");
}

// ** test: error_validate_success
TEST_CASE("Error validates successfully", "[domain][error]") {
    Error err("test", "Test Error");

    REQUIRE(err.validate());
}

// ** test: error_validate_empty_id
TEST_CASE("Error validate fails on empty id", "[domain][error]") {
    Error err("", "Test Error");

    REQUIRE_FALSE(err.validate());
}

// ** test: error_format_message_found
TEST_CASE("Error format_message returns matching language text", "[domain][error]") {
    Error err("test", "Test", {
        ErrorMessage{"en_US", "English message"},
        ErrorMessage{"es_ES", "Mensaje en español"}
    });

    REQUIRE(err.format_message("en_US") == "English message");
    REQUIRE(err.format_message("es_ES") == "Mensaje en español");
}

// ** test: error_format_message_not_found
TEST_CASE("Error format_message returns empty for unknown language", "[domain][error]") {
    Error err("test", "Test", {
        ErrorMessage{"en_US", "English"}
    });

    REQUIRE(err.format_message("fr_FR").empty());
}

// ** test: error_is_copyable
TEST_CASE("Error is copyable", "[domain][error]") {
    Error original("invalid_input", "Invalid Input", {
        ErrorMessage{"en_US", "Bad input"}
    });

    Error copy = original;

    REQUIRE(copy.id == "invalid_input");
    REQUIRE(copy.error_code == "INVALID_INPUT");
    REQUIRE(copy.message.size() == 1);
    REQUIRE(copy.message[0].text == "Bad input");
}
