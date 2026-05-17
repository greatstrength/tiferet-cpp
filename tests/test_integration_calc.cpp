// *** tests/test_integration_calc.cpp
// Full vertical integration test for the Tiferet framework.
// Wires YAML configs → repositories → DI container → registry →
// AppInterfaceContext → feature pipeline execution.
//
// This proves the complete Alpha 5 vertical works end-to-end.

#if TIFERET_HAS_YAML

#include <catch2/catch_test_macros.hpp>

#include <tiferet/contexts/app.h>
#include <tiferet/repos/feature.h>
#include <tiferet/repos/error.h>
#include <tiferet/repos/di.h>
#include <tiferet/repos/app.h>
#include <tiferet/assets/constants.h>

#include <fstream>
#include <cstdio>
#include <string>

using namespace tiferet;

namespace {

// *** helpers

static std::string temp_path(const std::string& name) {
    return std::string(std::tmpnam(nullptr)) + "_" + name + ".yml";
}

static void write_file(const std::string& path, const std::string& content) {
    std::ofstream out(path);
    out << content;
}

// *** fixtures

// ** fixture: add_event
/// Calculator event: adds "a" + "b".
class CalcAddEvent : public Service, public ExecutableEvent {
public:
    std::string execute(const DataContext& data, const DataContext& params) override {
        auto get = [&](const std::string& key) -> int {
            auto pit = params.find(key);
            if (pit != params.end()) return std::stoi(pit->second);
            auto dit = data.find(key);
            if (dit != data.end()) return std::stoi(dit->second);
            return 0;
        };
        return std::to_string(get("a") + get("b"));
    }
};

// ** fixture: subtract_event
/// Calculator event: subtracts "b" from "a".
class CalcSubtractEvent : public Service, public ExecutableEvent {
public:
    std::string execute(const DataContext& data, const DataContext& params) override {
        auto get = [&](const std::string& key) -> int {
            auto pit = params.find(key);
            if (pit != params.end()) return std::stoi(pit->second);
            auto dit = data.find(key);
            if (dit != data.end()) return std::stoi(dit->second);
            return 0;
        };
        return std::to_string(get("a") - get("b"));
    }
};

// ** fixture: multiply_event
/// Calculator event: multiplies "a" * "b".
class CalcMultiplyEvent : public Service, public ExecutableEvent {
public:
    std::string execute(const DataContext& data, const DataContext& params) override {
        auto get = [&](const std::string& key) -> int {
            auto pit = params.find(key);
            if (pit != params.end()) return std::stoi(pit->second);
            auto dit = data.find(key);
            if (dit != data.end()) return std::stoi(dit->second);
            return 0;
        };
        return std::to_string(get("a") * get("b"));
    }
};

// ** fixture: divide_event
/// Calculator event: divides "a" / "b". Raises DIVISION_BY_ZERO.
class CalcDivideEvent : public Service, public ExecutableEvent {
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
        if (b == 0) {
            DomainEvent::raise_error(
                error_codes::DIVISION_BY_ZERO,
                "Cannot divide by zero");
        }
        return std::to_string(a / b);
    }
};

// ** fixture: config_paths
struct ConfigPaths {
    std::string feature_yml;
    std::string container_yml;
    std::string error_yml;
    std::string app_yml;
};

// ** fixture: write_configs
/// Write all YAML config files for the calculator test.
static ConfigPaths write_configs() {
    ConfigPaths paths;

    // feature.yml
    paths.feature_yml = temp_path("integ_feature");
    write_file(paths.feature_yml,
        "features:\n"
        "  calc:\n"
        "    add:\n"
        "      name: Add Number\n"
        "      description: Adds two numbers\n"
        "      commands:\n"
        "        - attribute_id: add_event\n"
        "          name: Add a and b\n"
        "          data_key: result\n"
        "    subtract:\n"
        "      name: Subtract Number\n"
        "      commands:\n"
        "        - attribute_id: subtract_event\n"
        "          name: Subtract b from a\n"
        "          data_key: result\n"
        "    multiply:\n"
        "      name: Multiply Number\n"
        "      commands:\n"
        "        - attribute_id: multiply_event\n"
        "          name: Multiply a and b\n"
        "          data_key: result\n"
        "    divide:\n"
        "      name: Divide Number\n"
        "      commands:\n"
        "        - attribute_id: divide_event\n"
        "          name: Divide a by b\n"
        "          data_key: result\n"
        "    sqrt:\n"
        "      name: Square Root\n"
        "      description: Calculates the square root of a number\n"
        "      commands:\n"
        "        - attribute_id: multiply_event\n"
        "          name: Calculate square root of a\n"
        "          params:\n"
        "            b: '1'\n"
        "          data_key: result\n"
    );

    // container.yml
    paths.container_yml = temp_path("integ_container");
    write_file(paths.container_yml,
        "attrs:\n"
        "  add_event:\n"
        "    module_path: calc.events\n"
        "    class_name: AddNumber\n"
        "  subtract_event:\n"
        "    module_path: calc.events\n"
        "    class_name: SubtractNumber\n"
        "  multiply_event:\n"
        "    module_path: calc.events\n"
        "    class_name: MultiplyNumber\n"
        "  divide_event:\n"
        "    module_path: calc.events\n"
        "    class_name: DivideNumber\n"
    );

    // error.yml
    paths.error_yml = temp_path("integ_error");
    write_file(paths.error_yml,
        "errors:\n"
        "  invalid_input:\n"
        "    name: Invalid Numeric Input\n"
        "    message:\n"
        "      - lang: en_US\n"
        "        text: 'Value must be a number'\n"
        "      - lang: es_ES\n"
        "        text: 'El valor debe ser un numero'\n"
        "  division_by_zero:\n"
        "    name: Division By Zero\n"
        "    message:\n"
        "      - lang: en_US\n"
        "        text: 'Cannot divide by zero'\n"
        "      - lang: es_ES\n"
        "        text: 'No se puede dividir por cero'\n"
    );

    // app.yml
    paths.app_yml = temp_path("integ_app");
    write_file(paths.app_yml,
        "interfaces:\n"
        "  basic_calc:\n"
        "    name: Basic Calculator\n"
        "    description: Perform basic calculator operations\n"
    );

    return paths;
}

// ** fixture: cleanup_configs
static void cleanup_configs(const ConfigPaths& paths) {
    std::remove(paths.feature_yml.c_str());
    std::remove(paths.container_yml.c_str());
    std::remove(paths.error_yml.c_str());
    std::remove(paths.app_yml.c_str());
}

// ** fixture: build_registry
static ServiceRegistry build_registry() {
    ServiceRegistry registry;
    registry.register_service("calc.events::AddNumber",
        [](const ServiceParams&) { return std::make_unique<CalcAddEvent>(); });
    registry.register_service("calc.events::SubtractNumber",
        [](const ServiceParams&) { return std::make_unique<CalcSubtractEvent>(); });
    registry.register_service("calc.events::MultiplyNumber",
        [](const ServiceParams&) { return std::make_unique<CalcMultiplyEvent>(); });
    registry.register_service("calc.events::DivideNumber",
        [](const ServiceParams&) { return std::make_unique<CalcDivideEvent>(); });
    return registry;
}

} // anonymous namespace

// *** tests

// ** test: full_vertical_add
TEST_CASE("Integration: calculator add via YAML configs", "[integration]") {
    auto paths = write_configs();

    // Load repositories from YAML.
    FeatureYamlRepository features(paths.feature_yml);
    ErrorYamlRepository errors(paths.error_yml);
    DIYamlRepository di_configs(paths.container_yml);
    AppYamlRepository app_configs(paths.app_yml);

    // Build registry and DI container.
    auto registry = build_registry();
    DIContainer container(di_configs.list(), {"default"}, registry);

    // Create the app context.
    AppInterfaceContext app_ctx(features, errors, container);

    // Execute calc.add.
    DataContext data = {{"a", "3"}, {"b", "4"}};
    app_ctx.run("calc.add", data);

    REQUIRE(data["result"] == "7");

    cleanup_configs(paths);
}

// ** test: full_vertical_subtract
TEST_CASE("Integration: calculator subtract via YAML configs", "[integration]") {
    auto paths = write_configs();

    FeatureYamlRepository features(paths.feature_yml);
    ErrorYamlRepository errors(paths.error_yml);
    DIYamlRepository di_configs(paths.container_yml);
    auto registry = build_registry();
    DIContainer container(di_configs.list(), {"default"}, registry);
    AppInterfaceContext app_ctx(features, errors, container);

    DataContext data = {{"a", "10"}, {"b", "3"}};
    app_ctx.run("calc.subtract", data);

    REQUIRE(data["result"] == "7");

    cleanup_configs(paths);
}

// ** test: full_vertical_multiply
TEST_CASE("Integration: calculator multiply via YAML configs", "[integration]") {
    auto paths = write_configs();

    FeatureYamlRepository features(paths.feature_yml);
    ErrorYamlRepository errors(paths.error_yml);
    DIYamlRepository di_configs(paths.container_yml);
    auto registry = build_registry();
    DIContainer container(di_configs.list(), {"default"}, registry);
    AppInterfaceContext app_ctx(features, errors, container);

    DataContext data = {{"a", "5"}, {"b", "6"}};
    app_ctx.run("calc.multiply", data);

    REQUIRE(data["result"] == "30");

    cleanup_configs(paths);
}

// ** test: full_vertical_divide
TEST_CASE("Integration: calculator divide via YAML configs", "[integration]") {
    auto paths = write_configs();

    FeatureYamlRepository features(paths.feature_yml);
    ErrorYamlRepository errors(paths.error_yml);
    DIYamlRepository di_configs(paths.container_yml);
    auto registry = build_registry();
    DIContainer container(di_configs.list(), {"default"}, registry);
    AppInterfaceContext app_ctx(features, errors, container);

    DataContext data = {{"a", "8"}, {"b", "2"}};
    app_ctx.run("calc.divide", data);

    REQUIRE(data["result"] == "4");

    cleanup_configs(paths);
}

// ** test: full_vertical_sqrt_with_fixed_param
TEST_CASE("Integration: calculator sqrt uses fixed param from YAML", "[integration]") {
    auto paths = write_configs();

    FeatureYamlRepository features(paths.feature_yml);
    ErrorYamlRepository errors(paths.error_yml);
    DIYamlRepository di_configs(paths.container_yml);
    auto registry = build_registry();
    DIContainer container(di_configs.list(), {"default"}, registry);
    AppInterfaceContext app_ctx(features, errors, container);

    // sqrt reuses multiply with b=1 fixed param → a * 1 = a.
    DataContext data = {{"a", "16"}};
    app_ctx.run("calc.sqrt", data);

    REQUIRE(data["result"] == "16");

    cleanup_configs(paths);
}

#if TIFERET_EXCEPTIONS

// ** test: full_vertical_division_by_zero_with_error_context
TEST_CASE("Integration: division by zero + error formatting", "[integration]") {
    auto paths = write_configs();

    FeatureYamlRepository features(paths.feature_yml);
    ErrorYamlRepository errors(paths.error_yml);
    DIYamlRepository di_configs(paths.container_yml);
    auto registry = build_registry();
    DIContainer container(di_configs.list(), {"default"}, registry);
    AppInterfaceContext app_ctx(features, errors, container);

    DataContext data = {{"a", "8"}, {"b", "0"}};

    // Division by zero should throw.
    try {
        app_ctx.run("calc.divide", data);
        REQUIRE(false); // Should not reach here.
    } catch (const TiferetException& e) {
        REQUIRE(std::string(e.error_code()) == "DIVISION_BY_ZERO");

        // Use ErrorContext to format the error.
        std::string msg = app_ctx.error_context().format_error(
            e.error_code(), "en_US");
        REQUIRE(msg == "Cannot divide by zero");

        // Test multilingual formatting.
        std::string msg_es = app_ctx.error_context().format_error(
            e.error_code(), "es_ES");
        REQUIRE(msg_es == "No se puede dividir por cero");
    }

    cleanup_configs(paths);
}

// ** test: full_vertical_feature_not_found
TEST_CASE("Integration: feature not found throws", "[integration]") {
    auto paths = write_configs();

    FeatureYamlRepository features(paths.feature_yml);
    ErrorYamlRepository errors(paths.error_yml);
    DIYamlRepository di_configs(paths.container_yml);
    auto registry = build_registry();
    DIContainer container(di_configs.list(), {"default"}, registry);
    AppInterfaceContext app_ctx(features, errors, container);

    DataContext data;

    REQUIRE_THROWS_AS(
        app_ctx.run("calc.nonexistent", data),
        TiferetException
    );

    cleanup_configs(paths);
}

// ** test: error_context_standalone
TEST_CASE("Integration: ErrorContext formats errors from YAML", "[integration]") {
    auto paths = write_configs();

    ErrorYamlRepository errors(paths.error_yml);
    ErrorContext error_ctx(errors);

    // Lookup by id.
    REQUIRE(error_ctx.exists("invalid_input"));
    REQUIRE(error_ctx.format_error("invalid_input", "en_US") == "Value must be a number");

    // Lookup by error_code.
    REQUIRE(error_ctx.exists("DIVISION_BY_ZERO"));
    REQUIRE(error_ctx.format_error("DIVISION_BY_ZERO", "en_US") == "Cannot divide by zero");

    // Unknown error.
    REQUIRE(error_ctx.format_error("UNKNOWN_ERROR") == "");

    cleanup_configs(paths);
}

// ** test: app_repository_integration
TEST_CASE("Integration: AppYamlRepository loads interface config", "[integration]") {
    auto paths = write_configs();

    AppYamlRepository app_configs(paths.app_yml);

    const AppInterface* iface = app_configs.get("basic_calc");
    REQUIRE(iface != nullptr);
    REQUIRE(iface->name == "Basic Calculator");

    cleanup_configs(paths);
}

#endif // TIFERET_EXCEPTIONS

#endif // TIFERET_HAS_YAML
