// *** tests/test_mapper_settings.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/mappers/settings.h>
#include <tiferet/assets/constants.h>
#include <string>

using namespace tiferet;

// *** fixtures

// ** fixture: test_aggregate
/// Concrete aggregate for testing the base class behavior.
struct TestAggregate : Aggregate {
    std::string name;

    TestAggregate() = default;
    explicit TestAggregate(std::string name) : name(std::move(name)) {}

    void set_attribute(const std::string& attribute, const std::string& value) override {
        if (attribute == "name") { name = value; return; }

        // Fall through to base (raises INVALID_MODEL_ATTRIBUTE).
        Aggregate::set_attribute(attribute, value);
    }

    bool validate() const noexcept override { return !name.empty(); }
};

// ** fixture: test_transfer_object
/// Concrete transfer object for testing the base class behavior.
struct TestTransferObject : TransferObject {
    std::string name;
    std::string description;

    TestTransferObject() = default;
    TestTransferObject(std::string name, std::string description = "")
        : name(std::move(name)), description(std::move(description)) {}

    FieldMap to_map(const std::string& role = "") const override {
        FieldMap m;
        if (!name.empty()) m["name"] = name;
        if (!description.empty()) m["description"] = description;
        return m;
    }

    bool validate() const noexcept override { return !name.empty(); }
};

// *** tests: aggregate

// ** test: aggregate_set_attribute_success
TEST_CASE("Aggregate set_attribute updates known field", "[mappers][settings]") {
    TestAggregate agg("Original");

    agg.set_attribute("name", "Updated");

    REQUIRE(agg.name == "Updated");
}

#if TIFERET_EXCEPTIONS

// ** test: aggregate_set_attribute_unknown_throws
TEST_CASE("Aggregate set_attribute raises for unknown field", "[mappers][settings]") {
    TestAggregate agg("Test");

    REQUIRE_THROWS_AS(
        agg.set_attribute("nonexistent", "value"),
        TiferetException
    );

    // Verify the error code is INVALID_MODEL_ATTRIBUTE.
    try {
        agg.set_attribute("bad_field", "value");
    } catch (const TiferetException& e) {
        REQUIRE(std::string(e.error_code()) == error_codes::INVALID_MODEL_ATTRIBUTE);
    }
}

#endif // TIFERET_EXCEPTIONS

// *** tests: transfer_object

// ** test: transfer_object_to_map
TEST_CASE("TransferObject to_map returns expected keys", "[mappers][settings]") {
    TestTransferObject obj("Test Name", "A description");

    auto m = obj.to_map();

    REQUIRE(m.at("name") == "Test Name");
    REQUIRE(m.at("description") == "A description");
    REQUIRE(m.size() == 2);
}

// ** test: transfer_object_to_map_excludes_empty
TEST_CASE("TransferObject to_map excludes empty values", "[mappers][settings]") {
    TestTransferObject obj("Test Name");

    auto m = obj.to_map();

    REQUIRE(m.count("name") == 1);
    REQUIRE(m.count("description") == 0);
    REQUIRE(m.size() == 1);
}
