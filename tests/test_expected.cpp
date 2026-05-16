// *** tests/test_expected.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/assets/exceptions.h>
#include <tiferet/assets/expected.h>

using namespace tiferet;

// *** tests

// ** test: expected_value_state
TEST_CASE("Expected holds a value", "[alloc][expected]") {
    Expected<int> result(42);

    REQUIRE(result.has_value());
    REQUIRE(result.value() == 42);
}

// ** test: expected_error_state
TEST_CASE("Expected holds an error", "[alloc][expected]") {
    TiferetError err{"TEST_ERROR", "something went wrong"};
    Expected<int> result{Unexpected<TiferetError>{err}};

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().error_code != nullptr);
    REQUIRE(std::string(result.error().error_code) == "TEST_ERROR");
}

// ** test: expected_bool_conversion
TEST_CASE("Expected converts to bool", "[alloc][expected]") {
    Expected<int> good(10);
    Expected<int> bad{Unexpected<TiferetError>{TiferetError{"ERR"}}};

    REQUIRE(static_cast<bool>(good));
    REQUIRE_FALSE(static_cast<bool>(bad));
}

// ** test: expected_value_or
TEST_CASE("Expected value_or returns default on error", "[alloc][expected]") {
    Expected<int> bad{Unexpected<TiferetError>{TiferetError{"ERR"}}};

    REQUIRE(bad.value_or(-1) == -1);
}

// ** test: expected_value_or_returns_value
TEST_CASE("Expected value_or returns value when present", "[alloc][expected]") {
    Expected<int> good(42);

    REQUIRE(good.value_or(-1) == 42);
}

// ** test: expected_void_success
TEST_CASE("Expected<void> success state", "[alloc][expected]") {
    Expected<void> result;

    REQUIRE(result.has_value());
}

// ** test: expected_void_error
TEST_CASE("Expected<void> error state", "[alloc][expected]") {
    Expected<void> result{Unexpected<TiferetError>{TiferetError{"VOID_ERR"}}};

    REQUIRE_FALSE(result.has_value());
    REQUIRE(std::string(result.error().error_code) == "VOID_ERR");
}

// ** test: expected_copy
TEST_CASE("Expected is copyable", "[alloc][expected]") {
    Expected<int> original(42);
    Expected<int> copy = original;

    REQUIRE(copy.has_value());
    REQUIRE(copy.value() == 42);
}

// ** test: expected_move
TEST_CASE("Expected is movable", "[alloc][expected]") {
    Expected<int> original(42);
    Expected<int> moved = std::move(original);

    REQUIRE(moved.has_value());
    REQUIRE(moved.value() == 42);
}
