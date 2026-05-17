// *** tests/test_di_container.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/contexts/di.h>
#include <tiferet/assets/constants.h>

using namespace tiferet;

namespace {

// *** fixtures

// ** fixture: adder_service
class AdderService : public Service {
public:
    std::string label = "adder";
};

// ** fixture: multiplier_service
class MultiplierService : public Service {
public:
    std::string label = "multiplier";
};

// ** fixture: flagged_adder_service
class FlaggedAdderService : public Service {
public:
    std::string label = "flagged_adder";
};

// ** fixture: build_registry
/// Helper to build a pre-populated registry for testing.
static ServiceRegistry build_registry() {
    ServiceRegistry registry;
    registry.register_service("calc.events::Adder", [](const ServiceParams&) {
        return std::make_unique<AdderService>();
    });
    registry.register_service("calc.events::Multiplier", [](const ServiceParams&) {
        return std::make_unique<MultiplierService>();
    });
    registry.register_service("calc.events::FlaggedAdder", [](const ServiceParams&) {
        return std::make_unique<FlaggedAdderService>();
    });
    return registry;
}

} // anonymous namespace

// *** tests

// ** test: resolve_default_type
TEST_CASE("DIContainer resolves service from default module_path/class_name", "[di]") {
    auto registry = build_registry();
    std::vector<ServiceConfiguration> configs = {
        ServiceConfiguration("add_event", "", "calc.events", "Adder")
    };

    DIContainer container(configs, {"default"}, registry);

    auto* svc = container.get_service("add_event");
    REQUIRE(svc != nullptr);

    auto* adder = dynamic_cast<AdderService*>(svc);
    REQUIRE(adder != nullptr);
    REQUIRE(adder->label == "adder");
}

// ** test: resolve_multiple_configs
TEST_CASE("DIContainer resolves multiple service configurations", "[di]") {
    auto registry = build_registry();
    std::vector<ServiceConfiguration> configs = {
        ServiceConfiguration("add_event", "", "calc.events", "Adder"),
        ServiceConfiguration("mul_event", "", "calc.events", "Multiplier"),
    };

    DIContainer container(configs, {"default"}, registry);

    REQUIRE(container.size() == 2);
    REQUIRE(container.has("add_event"));
    REQUIRE(container.has("mul_event"));
}

// ** test: resolve_flagged_dependency
TEST_CASE("DIContainer resolves flagged dependency over default", "[di]") {
    auto registry = build_registry();

    ServiceConfiguration config(
        "add_event", "",
        "calc.events", "Adder", {},
        {FlaggedDependency("calc.events", "FlaggedAdder", "test_flag")}
    );

    DIContainer container({config}, {"test_flag"}, registry);

    auto* svc = container.get_service("add_event");
    REQUIRE(svc != nullptr);

    auto* flagged = dynamic_cast<FlaggedAdderService*>(svc);
    REQUIRE(flagged != nullptr);
    REQUIRE(flagged->label == "flagged_adder");
}

// ** test: flag_priority_order
TEST_CASE("DIContainer respects flag priority order", "[di]") {
    auto registry = build_registry();

    ServiceConfiguration config(
        "add_event", "",
        "calc.events", "Adder", {},
        {
            FlaggedDependency("calc.events", "FlaggedAdder", "low_priority"),
            FlaggedDependency("calc.events", "Multiplier", "high_priority"),
        }
    );

    // high_priority listed first in flags → should win.
    DIContainer container({config}, {"high_priority", "low_priority"}, registry);

    auto* svc = container.get_service("add_event");
    auto* mul = dynamic_cast<MultiplierService*>(svc);

    REQUIRE(mul != nullptr);
    REQUIRE(mul->label == "multiplier");
}

// ** test: flag_not_matched_falls_to_default
TEST_CASE("DIContainer falls to default when no flag matches", "[di]") {
    auto registry = build_registry();

    ServiceConfiguration config(
        "add_event", "",
        "calc.events", "Adder", {},
        {FlaggedDependency("calc.events", "FlaggedAdder", "unmatched_flag")}
    );

    DIContainer container({config}, {"other_flag"}, registry);

    auto* svc = container.get_service("add_event");
    auto* adder = dynamic_cast<AdderService*>(svc);

    REQUIRE(adder != nullptr);
    REQUIRE(adder->label == "adder");
}

// ** test: get_service_returns_nullptr_for_unknown_id
TEST_CASE("DIContainer::get_service returns nullptr for unknown ID", "[di]") {
    auto registry = build_registry();

    DIContainer container({}, {}, registry);

    REQUIRE(container.get_service("nonexistent") == nullptr);
}

// ** test: has_and_size
TEST_CASE("DIContainer::has and size reflect resolved state", "[di]") {
    auto registry = build_registry();
    std::vector<ServiceConfiguration> configs = {
        ServiceConfiguration("svc_a", "", "calc.events", "Adder"),
    };

    DIContainer container(configs, {}, registry);

    REQUIRE(container.has("svc_a"));
    REQUIRE_FALSE(container.has("svc_b"));
    REQUIRE(container.size() == 1);
}

#if TIFERET_EXCEPTIONS

// ** test: resolve_missing_type_throws
TEST_CASE("DIContainer throws SERVICE_RESOLUTION_FAILED when no type available", "[di]") {
    ServiceRegistry registry;

    // Config with no module_path/class_name and no flagged dependencies.
    ServiceConfiguration config("empty_service");

    REQUIRE_THROWS_AS(
        DIContainer({config}, {}, registry),
        TiferetException
    );
}

// ** test: resolve_missing_type_error_code
TEST_CASE("DIContainer resolution failure carries correct error code", "[di]") {
    ServiceRegistry registry;
    ServiceConfiguration config("bad_service");

    try {
        DIContainer({config}, {}, registry);
        FAIL("Expected TiferetException");
    } catch (const TiferetException& e) {
        REQUIRE(std::string(e.error_code()) == error_codes::SERVICE_RESOLUTION_FAILED);
    }
}

// ** test: resolve_unregistered_key_throws
TEST_CASE("DIContainer throws SERVICE_NOT_REGISTERED when key not in registry", "[di]") {
    ServiceRegistry registry;
    // Config points to a type that isn't registered.
    ServiceConfiguration config("missing", "", "unknown.module", "UnknownClass");

    REQUIRE_THROWS_AS(
        DIContainer({config}, {}, registry),
        TiferetException
    );
}

#endif // TIFERET_EXCEPTIONS
