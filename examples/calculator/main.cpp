// *** examples/calculator/main.cpp
// A simple calculator demonstrating the Tiferet C++ framework.
//
// This example shows the complete vertical:
//   YAML configs → repositories → DI container → registry →
//   AppInterfaceContext → feature pipeline execution.
//
// Build:
//   cmake -S ../.. -B build -G Ninja \
//     -DTIFERET_BUILD_TESTS=OFF -DTIFERET_EXCEPTIONS=ON -DTIFERET_YAML=ON
//   ninja -C build calculator
//
// Run:
//   ./build/examples/calculator/calculator

#include <tiferet/tiferet.h>

#include <iostream>
#include <string>

using namespace tiferet;

// *** events

// ** event: add_event
class AddEvent : public Service, public ExecutableEvent {
public:
    std::string execute(const DataContext& data, const DataContext& params) override {
        auto get = [&](const std::string& key) -> double {
            auto pit = params.find(key);
            if (pit != params.end()) return std::stod(pit->second);
            auto dit = data.find(key);
            if (dit != data.end()) return std::stod(dit->second);
            return 0;
        };
        return std::to_string(get("a") + get("b"));
    }
};

// ** event: subtract_event
class SubtractEvent : public Service, public ExecutableEvent {
public:
    std::string execute(const DataContext& data, const DataContext& params) override {
        auto get = [&](const std::string& key) -> double {
            auto pit = params.find(key);
            if (pit != params.end()) return std::stod(pit->second);
            auto dit = data.find(key);
            if (dit != data.end()) return std::stod(dit->second);
            return 0;
        };
        return std::to_string(get("a") - get("b"));
    }
};

// ** event: multiply_event
class MultiplyEvent : public Service, public ExecutableEvent {
public:
    std::string execute(const DataContext& data, const DataContext& params) override {
        auto get = [&](const std::string& key) -> double {
            auto pit = params.find(key);
            if (pit != params.end()) return std::stod(pit->second);
            auto dit = data.find(key);
            if (dit != data.end()) return std::stod(dit->second);
            return 0;
        };
        return std::to_string(get("a") * get("b"));
    }
};

// ** event: divide_event
class DivideEvent : public Service, public ExecutableEvent {
public:
    std::string execute(const DataContext& data, const DataContext& params) override {
        auto get = [&](const std::string& key) -> double {
            auto pit = params.find(key);
            if (pit != params.end()) return std::stod(pit->second);
            auto dit = data.find(key);
            if (dit != data.end()) return std::stod(dit->second);
            return 0;
        };
        double a = get("a");
        double b = get("b");
        if (b == 0.0) {
            DomainEvent::raise_error(
                error_codes::DIVISION_BY_ZERO,
                "Cannot divide by zero");
        }
        return std::to_string(a / b);
    }
};

// *** main

int main() {
    // Path to config files (relative to working directory).
    const std::string config_dir = "examples/calculator/configs/";

    // Load repositories from YAML configuration.
    FeatureYamlRepository features(config_dir + "feature.yml");
    ErrorYamlRepository errors(config_dir + "error.yml");
    DIYamlRepository di_configs(config_dir + "container.yml");

    // Register event factories in the service registry.
    ServiceRegistry registry;
    registry.register_service("calc.events::AddEvent",
        [](const ServiceParams&) { return std::make_unique<AddEvent>(); });
    registry.register_service("calc.events::SubtractEvent",
        [](const ServiceParams&) { return std::make_unique<SubtractEvent>(); });
    registry.register_service("calc.events::MultiplyEvent",
        [](const ServiceParams&) { return std::make_unique<MultiplyEvent>(); });
    registry.register_service("calc.events::DivideEvent",
        [](const ServiceParams&) { return std::make_unique<DivideEvent>(); });

    // Build the DI container from YAML service configurations.
    DIContainer container(di_configs.list(), {"default"}, registry);

    // Create the application context.
    AppInterfaceContext app(features, errors, container);

    // Define test cases.
    struct TestCase {
        std::string feature_id;
        DataContext data;
        std::string format;
    };

    std::vector<TestCase> cases = {
        {"calc.add",      {{"a", "1"}, {"b", "2"}},  "{} + {} = {}"},
        {"calc.subtract", {{"a", "5"}, {"b", "3"}},  "{} - {} = {}"},
        {"calc.multiply", {{"a", "4"}, {"b", "3"}},  "{} * {} = {}"},
        {"calc.divide",   {{"a", "8"}, {"b", "2"}},  "{} / {} = {}"},
        {"calc.divide",   {{"a", "8"}, {"b", "0"}},  "{} / {} = {}"},  // division by zero
    };

    // Execute each test case.
    for (auto& tc : cases) {
        try {
            app.run(tc.feature_id, tc.data);

            std::cout << tc.data["a"] << " op " << tc.data["b"]
                      << " = " << tc.data["result"] << std::endl;

        } catch (const TiferetException& e) {
            // Format the error using the ErrorContext.
            std::string msg = app.error_context().format_error(e.error_code());
            if (msg.empty()) msg = e.what();
            std::cout << "Error: " << msg << std::endl;
        }
    }

    return 0;
}
