// *** tests/test_domain_object.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/domain/settings.h>
#include <string>

using namespace tiferet;

// *** fixtures

// ** fixture: sample_domain_object
/// A concrete domain object for testing.
struct SampleDomainObject : DomainObject {
    std::string id;
    std::string name;

    SampleDomainObject(std::string id, std::string name)
        : id(std::move(id)), name(std::move(name)) {}

    bool validate() const noexcept override {
        return !id.empty() && !name.empty();
    }
};

// *** tests

// ** test: domain_object_validate_success
TEST_CASE("DomainObject validates successfully", "[domain]") {
    SampleDomainObject obj("test.id", "Test Name");

    REQUIRE(obj.validate());
}

// ** test: domain_object_validate_failure
TEST_CASE("DomainObject validates failure on empty fields", "[domain]") {
    SampleDomainObject obj("", "Test Name");

    REQUIRE_FALSE(obj.validate());
}

// ** test: domain_object_is_copyable
TEST_CASE("DomainObject is copyable", "[domain]") {
    SampleDomainObject original("a.b", "AB");
    SampleDomainObject copy = original;

    REQUIRE(copy.id == "a.b");
    REQUIRE(copy.name == "AB");
}
