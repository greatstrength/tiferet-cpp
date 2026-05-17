// *** tests/test_registry.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/contexts/registry.h>
#include <tiferet/assets/constants.h>

using namespace tiferet;

namespace {

// *** fixtures

// ** fixture: dummy_service
/// A minimal concrete Service for testing.
class DummyService : public Service {
public:
    std::string tag;
    DummyService() : tag("default") {}
    explicit DummyService(const std::string& t) : tag(t) {}
};

// ** fixture: param_aware_service
/// A Service that reads construction parameters.
class ParamAwareService : public Service {
public:
    std::string config_value;
    explicit ParamAwareService(const std::string& val) : config_value(val) {}
};

} // anonymous namespace

// *** tests

// ** test: make_key_format
TEST_CASE("ServiceRegistry::make_key produces module::class format", "[registry]") {
    auto key = ServiceRegistry::make_key("calc.events", "AddNumber");

    REQUIRE(key == "calc.events::AddNumber");
}

// ** test: make_key_empty
TEST_CASE("ServiceRegistry::make_key handles empty strings", "[registry]") {
    auto key = ServiceRegistry::make_key("", "");

    REQUIRE(key == "::");
}

// ** test: register_and_create
TEST_CASE("ServiceRegistry register and create returns a Service", "[registry]") {
    ServiceRegistry registry;
    registry.register_service("test::Dummy", [](const ServiceParams&) {
        return std::make_unique<DummyService>();
    });

    auto svc = registry.create_service("test::Dummy");

    REQUIRE(svc != nullptr);
    auto* dummy = dynamic_cast<DummyService*>(svc.get());
    REQUIRE(dummy != nullptr);
    REQUIRE(dummy->tag == "default");
}

// ** test: create_with_params
TEST_CASE("ServiceRegistry factory receives params", "[registry]") {
    ServiceRegistry registry;
    registry.register_service("test::ParamAware", [](const ServiceParams& params) {
        auto it = params.find("value");
        std::string val = (it != params.end()) ? it->second : "none";
        return std::make_unique<ParamAwareService>(val);
    });

    auto svc = registry.create_service("test::ParamAware", {{"value", "hello"}});

    auto* pa = dynamic_cast<ParamAwareService*>(svc.get());
    REQUIRE(pa != nullptr);
    REQUIRE(pa->config_value == "hello");
}

// ** test: has_returns_true
TEST_CASE("ServiceRegistry::has returns true for registered keys", "[registry]") {
    ServiceRegistry registry;
    registry.register_service("a::B", [](const ServiceParams&) {
        return std::make_unique<DummyService>();
    });

    REQUIRE(registry.has("a::B"));
}

// ** test: has_returns_false
TEST_CASE("ServiceRegistry::has returns false for unregistered keys", "[registry]") {
    ServiceRegistry registry;

    REQUIRE_FALSE(registry.has("no::Such"));
}

// ** test: size
TEST_CASE("ServiceRegistry::size reflects registrations", "[registry]") {
    ServiceRegistry registry;

    REQUIRE(registry.size() == 0);

    registry.register_service("a::B", [](const ServiceParams&) {
        return std::make_unique<DummyService>();
    });

    REQUIRE(registry.size() == 1);
}

// ** test: overwrite_registration
TEST_CASE("ServiceRegistry overwrites duplicate key", "[registry]") {
    ServiceRegistry registry;
    registry.register_service("test::Dup", [](const ServiceParams&) {
        return std::make_unique<DummyService>("first");
    });
    registry.register_service("test::Dup", [](const ServiceParams&) {
        return std::make_unique<DummyService>("second");
    });

    auto svc = registry.create_service("test::Dup");
    auto* dummy = dynamic_cast<DummyService*>(svc.get());

    REQUIRE(dummy->tag == "second");
    REQUIRE(registry.size() == 1);
}

// ** test: macro_registration
TEST_CASE("TIFERET_REGISTER_SERVICE macro registers a default factory", "[registry]") {
    ServiceRegistry registry;
    TIFERET_REGISTER_SERVICE(registry, "macro::Dummy", DummyService);

    REQUIRE(registry.has("macro::Dummy"));

    auto svc = registry.create_service("macro::Dummy");
    auto* dummy = dynamic_cast<DummyService*>(svc.get());

    REQUIRE(dummy != nullptr);
    REQUIRE(dummy->tag == "default");
}

#if TIFERET_EXCEPTIONS

// ** test: create_unknown_key_throws
TEST_CASE("ServiceRegistry::create_service throws for unknown key", "[registry]") {
    ServiceRegistry registry;

    REQUIRE_THROWS_AS(
        registry.create_service("no::Such"),
        TiferetException
    );
}

// ** test: create_unknown_key_error_code
TEST_CASE("ServiceRegistry::create_service carries SERVICE_NOT_REGISTERED error code", "[registry]") {
    ServiceRegistry registry;

    try {
        registry.create_service("missing::Key");
        FAIL("Expected TiferetException");
    } catch (const TiferetException& e) {
        REQUIRE(std::string(e.error_code()) == error_codes::SERVICE_NOT_REGISTERED);
    }
}

#endif // TIFERET_EXCEPTIONS
