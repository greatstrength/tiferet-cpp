// *** tests/test_feature_context.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/contexts/feature.h>
#include <tiferet/assets/constants.h>

#include <sstream>

using namespace tiferet;

namespace {

// *** fixtures

// ** fixture: add_event
/// An ExecutableEvent that adds two numbers from the data context.
/// Reads "a" and "b", returns their sum as a string.
class AddEvent : public Service, public ExecutableEvent {
public:
    std::string execute(const DataContext& data, const DataContext& params) override {

        // Read operands: params override data.
        auto get = [&](const std::string& key) -> int {
            auto pit = params.find(key);
            if (pit != params.end()) return std::stoi(pit->second);
            auto dit = data.find(key);
            if (dit != data.end()) return std::stoi(dit->second);
            return 0;
        };

        int a = get("a");
        int b = get("b");
        return std::to_string(a + b);
    }
};

// ** fixture: multiply_event
/// An ExecutableEvent that multiplies "a" and "b".
class MultiplyEvent : public Service, public ExecutableEvent {
public:
    std::string execute(const DataContext& data, const DataContext& params) override {

        auto get = [&](const std::string& key) -> int {
            auto pit = params.find(key);
            if (pit != params.end()) return std::stoi(pit->second);
            auto dit = data.find(key);
            if (dit != data.end()) return std::stoi(dit->second);
            return 0;
        };

        int a = get("a");
        int b = get("b");
        return std::to_string(a * b);
    }
};

// ** fixture: error_event
/// An ExecutableEvent that always raises an error.
class ErrorEvent : public Service, public ExecutableEvent {
public:
    std::string execute(const DataContext&, const DataContext&) override {
        DomainEvent::raise_error(error_codes::APP_ERROR, "intentional failure");
        return ""; // unreachable
    }
};

// ** fixture: append_event
/// An ExecutableEvent that appends a suffix to a "text" field.
class AppendEvent : public Service, public ExecutableEvent {
public:
    std::string execute(const DataContext& data, const DataContext& params) override {

        auto dit = data.find("text");
        std::string text = (dit != data.end()) ? dit->second : "";

        auto pit = params.find("suffix");
        std::string suffix = (pit != params.end()) ? pit->second : "";

        return text + suffix;
    }
};

// ** fixture: non_executable_service
/// A service that does NOT implement ExecutableEvent.
class NonExecutableService : public Service {};

// ** fixture: build_test_registry
static ServiceRegistry build_test_registry() {
    ServiceRegistry registry;
    registry.register_service("calc::AddEvent", [](const ServiceParams&) {
        return std::make_unique<AddEvent>();
    });
    registry.register_service("calc::MultiplyEvent", [](const ServiceParams&) {
        return std::make_unique<MultiplyEvent>();
    });
    registry.register_service("calc::ErrorEvent", [](const ServiceParams&) {
        return std::make_unique<ErrorEvent>();
    });
    registry.register_service("text::AppendEvent", [](const ServiceParams&) {
        return std::make_unique<AppendEvent>();
    });
    registry.register_service("test::NonExecutable", [](const ServiceParams&) {
        return std::make_unique<NonExecutableService>();
    });
    return registry;
}

// ** fixture: build_container
static DIContainer build_container(const ServiceRegistry& registry,
                                   const std::vector<ServiceConfiguration>& configs) {
    return DIContainer(configs, {"default"}, registry);
}

} // anonymous namespace

// *** tests

// ** test: single_step_execution
TEST_CASE("FeatureContext executes a single-step feature", "[feature]") {
    auto registry = build_test_registry();
    std::vector<ServiceConfiguration> configs = {
        ServiceConfiguration("add_event", "", "calc", "AddEvent"),
    };
    auto container = build_container(registry, configs);
    FeatureContext ctx(container);

    Feature feature("calc.add", "Add", "calc", "add", {
        FeatureEvent("Add a + b", "add_event", {}, "result")
    });

    DataContext data = {{"a", "3"}, {"b", "4"}};
    ctx.execute_feature(feature, data);

    REQUIRE(data["result"] == "7");
}

// ** test: multi_step_data_flow
TEST_CASE("FeatureContext flows data between steps", "[feature]") {
    auto registry = build_test_registry();
    std::vector<ServiceConfiguration> configs = {
        ServiceConfiguration("add_event", "", "calc", "AddEvent"),
        ServiceConfiguration("mul_event", "", "calc", "MultiplyEvent"),
    };
    auto container = build_container(registry, configs);
    FeatureContext ctx(container);

    // Step 1: add a+b → store in "sum"
    // Step 2: multiply sum * b (sum is read from data as "a")
    Feature feature("calc.pipeline", "Pipeline", "calc", "pipeline", {
        FeatureEvent("Add", "add_event", {}, "sum"),
        FeatureEvent("Multiply", "mul_event", {{"a", "0"}}, "product"),
    });

    DataContext data = {{"a", "3"}, {"b", "4"}};
    ctx.execute_feature(feature, data);

    // Step 1: 3 + 4 = 7
    REQUIRE(data["sum"] == "7");
    // Step 2: a=0 (from params), b=4 → 0 * 4 = 0
    REQUIRE(data["product"] == "0");
}

// ** test: step_params_override_data
TEST_CASE("Step parameters override data context values", "[feature]") {
    auto registry = build_test_registry();
    std::vector<ServiceConfiguration> configs = {
        ServiceConfiguration("add_event", "", "calc", "AddEvent"),
    };
    auto container = build_container(registry, configs);
    FeatureContext ctx(container);

    Feature feature("calc.override", "Override", "calc", "override", {
        FeatureEvent("Add with override", "add_event",
                     {{"b", "100"}}, "result"),
    });

    DataContext data = {{"a", "5"}, {"b", "2"}};
    ctx.execute_feature(feature, data);

    // b is overridden to 100 by step params → 5 + 100 = 105
    REQUIRE(data["result"] == "105");
}

// ** test: no_data_key_no_store
TEST_CASE("Empty data_key does not store result", "[feature]") {
    auto registry = build_test_registry();
    std::vector<ServiceConfiguration> configs = {
        ServiceConfiguration("add_event", "", "calc", "AddEvent"),
    };
    auto container = build_container(registry, configs);
    FeatureContext ctx(container);

    Feature feature("calc.noop", "NoStore", "calc", "noop", {
        FeatureEvent("Add without store", "add_event"),
    });

    DataContext data = {{"a", "1"}, {"b", "2"}};
    ctx.execute_feature(feature, data);

    REQUIRE(data.find("result") == data.end());
}

// ** test: append_chain
TEST_CASE("FeatureContext chains text processing steps", "[feature]") {
    auto registry = build_test_registry();
    std::vector<ServiceConfiguration> configs = {
        ServiceConfiguration("append_event", "", "text", "AppendEvent"),
    };
    auto container = build_container(registry, configs);
    FeatureContext ctx(container);

    Feature feature("text.chain", "Chain", "text", "chain", {
        FeatureEvent("Append hello", "append_event",
                     {{"suffix", " world"}}, "text"),
        FeatureEvent("Append excl", "append_event",
                     {{"suffix", "!"}}, "text"),
    });

    DataContext data = {{"text", "hello"}};
    ctx.execute_feature(feature, data);

    REQUIRE(data["text"] == "hello world!");
}

#if TIFERET_EXCEPTIONS

// ** test: pass_on_error_continues
TEST_CASE("FeatureContext pass_on_error=true continues after error", "[feature]") {
    auto registry = build_test_registry();
    std::vector<ServiceConfiguration> configs = {
        ServiceConfiguration("error_event", "", "calc", "ErrorEvent"),
        ServiceConfiguration("add_event", "", "calc", "AddEvent"),
    };
    auto container = build_container(registry, configs);
    FeatureContext ctx(container);

    Feature feature("calc.resilient", "Resilient", "calc", "resilient", {
        FeatureEvent("Fail", "error_event", {}, "", true),  // pass_on_error=true
        FeatureEvent("Add", "add_event", {}, "result"),
    });

    DataContext data = {{"a", "10"}, {"b", "20"}};
    ctx.execute_feature(feature, data);

    // The error step was swallowed, addition proceeds.
    REQUIRE(data["result"] == "30");
}

// ** test: pass_on_error_false_propagates
TEST_CASE("FeatureContext pass_on_error=false propagates error", "[feature]") {
    auto registry = build_test_registry();
    std::vector<ServiceConfiguration> configs = {
        ServiceConfiguration("error_event", "", "calc", "ErrorEvent"),
        ServiceConfiguration("add_event", "", "calc", "AddEvent"),
    };
    auto container = build_container(registry, configs);
    FeatureContext ctx(container);

    Feature feature("calc.strict", "Strict", "calc", "strict", {
        FeatureEvent("Fail", "error_event"),      // pass_on_error=false (default)
        FeatureEvent("Add", "add_event", {}, "result"),
    });

    DataContext data = {{"a", "10"}, {"b", "20"}};

    REQUIRE_THROWS_AS(
        ctx.execute_feature(feature, data),
        TiferetException
    );

    // Second step never runs.
    REQUIRE(data.find("result") == data.end());
}

// ** test: missing_service_throws
TEST_CASE("FeatureContext throws when service_id not in container", "[feature]") {
    auto registry = build_test_registry();
    auto container = build_container(registry, {});
    FeatureContext ctx(container);

    Feature feature("calc.bad", "Bad", "calc", "bad", {
        FeatureEvent("Missing", "nonexistent_event"),
    });

    DataContext data;

    REQUIRE_THROWS_AS(
        ctx.execute_feature(feature, data),
        TiferetException
    );
}

// ** test: non_executable_service_throws
TEST_CASE("FeatureContext throws when service is not ExecutableEvent", "[feature]") {
    auto registry = build_test_registry();
    std::vector<ServiceConfiguration> configs = {
        ServiceConfiguration("non_exec", "", "test", "NonExecutable"),
    };
    auto container = build_container(registry, configs);
    FeatureContext ctx(container);

    Feature feature("test.cast_fail", "CastFail", "test", "cast_fail", {
        FeatureEvent("Bad cast", "non_exec"),
    });

    DataContext data;

    REQUIRE_THROWS_AS(
        ctx.execute_feature(feature, data),
        TiferetException
    );
}

#endif // TIFERET_EXCEPTIONS
