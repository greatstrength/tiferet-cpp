// *** tests/test_domain_event.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/events/settings.h>
#include <tiferet/assets/constants.h>

using namespace tiferet;

// *** fixtures

// ** fixture: add_params
struct AddParams {
    int a;
    int b;
};

// ** fixture: add_number_event
/// A concrete typed domain event for testing.
class AddNumberEvent : public TypedDomainEvent<AddParams, int> {
public:
    int execute(const AddParams& params) override {
        return params.a + params.b;
    }
};

// ** fixture: divide_params
struct DivideParams {
    int a;
    int b;
};

// ** fixture: divide_number_event
/// A domain event that uses verify() to prevent division by zero.
class DivideNumberEvent : public TypedDomainEvent<DivideParams, int> {
public:
    int execute(const DivideParams& params) override {
        verify(params.b != 0, error_codes::DIVISION_BY_ZERO, "Cannot divide by zero");
        return params.a / params.b;
    }
};

// *** tests

// ** test: typed_event_execute
TEST_CASE("TypedDomainEvent executes with typed params", "[events]") {
    AddNumberEvent event;
    int result = event.execute(AddParams{3, 4});

    REQUIRE(result == 7);
}

// ** test: handle_static_method
TEST_CASE("DomainEvent::handle invokes execute", "[events]") {
    AddNumberEvent event;
    int result = DomainEvent::handle(event, AddParams{10, 20});

    REQUIRE(result == 30);
}

// ** test: handle_rvalue_event
TEST_CASE("DomainEvent::handle works with rvalue event", "[events]") {
    int result = DomainEvent::handle(AddNumberEvent{}, AddParams{5, 5});

    REQUIRE(result == 10);
}

#if TIFERET_EXCEPTIONS

// ** test: verify_throws_on_failure
TEST_CASE("DomainEvent verify throws TiferetException on failure", "[events]") {
    DivideNumberEvent event;

    REQUIRE_THROWS_AS(
        event.execute(DivideParams{10, 0}),
        TiferetException
    );
}

// ** test: verify_passes_on_success
TEST_CASE("DomainEvent verify passes on success", "[events]") {
    DivideNumberEvent event;
    int result = event.execute(DivideParams{10, 2});

    REQUIRE(result == 5);
}

// ** test: raise_error_throws
TEST_CASE("DomainEvent::raise_error throws TiferetException", "[events]") {
    REQUIRE_THROWS_AS(
        DomainEvent::raise_error(error_codes::APP_ERROR, "test"),
        TiferetException
    );
}

// ** test: verify_error_code_is_correct
TEST_CASE("verify failure carries correct error code", "[events]") {
    DivideNumberEvent event;

    try {
        event.execute(DivideParams{1, 0});
        FAIL("Expected TiferetException");
    } catch (const TiferetException& e) {
        REQUIRE(std::string(e.error_code()) == "DIVISION_BY_ZERO");
    }
}

#endif // TIFERET_EXCEPTIONS
