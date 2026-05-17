# tiferet-cpp

A C++ framework for Domain-Driven Design in firmware and embedded systems — deterministic memory, zero-overhead abstractions, and configuration-driven workflows for bare-metal to RTOS targets.

## Architecture

```
tiferet/
├── assets/       — Constants, error types (TiferetError/TiferetException), Expected<T>, alloc config
├── domain/       — DomainObject base and domain models (Error, Feature, AppInterface, ServiceConfiguration)
├── events/       — DomainEvent base with verify(), raise_error(), handle()
├── interfaces/   — Service (ABC) base and service contracts (Feature, Error, Container, App)
├── mappers/      — Aggregate (mutation) and TransferObject (serialization) MI mixins
├── contexts/     — Runtime orchestration (Registry, DIContainer, FeatureContext, ErrorContext, AppInterfaceContext)
├── repos/        — YAML-backed Service implementations (Feature, Error, DI, App)
└── utils/        — StaticString<N>, StaticVector<T,N>, ArenaAllocator<T>, YamlLoader
```

### Key Concepts

- **DomainObject** — Read-only value types. Mutation goes through Aggregates in the mappers layer.
- **DomainEvent** — Base class for domain operations with `verify()`, `raise_error()`, and `handle()`.
- **Service** — Pure virtual base for all service contracts. Events receive services via DI.
- **Aggregate** — MI mixin combining a domain model with validated `set_attribute()` mutation.
- **TransferObject** — MI mixin combining a domain model with `to_map()` serialization.
- **ServiceRegistry** — Maps `module_path::class_name` strings to factory functions (C++ equivalent of Python's `import_module + getattr`).
- **DIContainer** — Resolves `ServiceConfiguration` entries into live `Service*` instances via the registry with flag-based dependency selection.
- **FeatureContext** — Executes multi-step feature pipelines via `ExecutableEvent` interface.
- **AppInterfaceContext** — Top-level entry point: `run(feature_id, data)`.

### Runtime Flow

1. YAML configs are loaded by repositories (`FeatureYamlRepository`, `ErrorYamlRepository`, `DIYamlRepository`).
2. Service factories are registered in `ServiceRegistry`.
3. `DIContainer` resolves configurations into live services.
4. `AppInterfaceContext.run()` looks up a feature and executes its step pipeline.
5. Each step resolves a service from the DI container, casts to `ExecutableEvent`, and executes.
6. Results flow through the `DataContext` (string-keyed map).

## Build

Requires **C++17**, **CMake ≥ 3.16**, and a C++ compiler (GCC, Clang, MSVC).

```bash
# Configure (fetches yaml-cpp and Catch2 on first run)
cmake -S . -B build -G Ninja \
  -DTIFERET_BUILD_TESTS=ON \
  -DTIFERET_EXCEPTIONS=ON \
  -DTIFERET_YAML=ON

# Build
ninja -C build

# Test
ctest --test-dir build --output-on-failure
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `TIFERET_ALLOC_MODE` | `HEAP` | Allocation mode: `HEAP`, `ARENA`, or `STATIC` |
| `TIFERET_EXCEPTIONS` | `ON` | Enable C++ exceptions |
| `TIFERET_YAML` | `ON` | Enable YAML utilities (fetches yaml-cpp 0.8.0) |
| `TIFERET_BUILD_TESTS` | `ON` | Build Catch2 unit tests |
| `TIFERET_BUILD_EXAMPLES` | `ON` | Build example applications |

### Header-Only

The framework is header-only. Link against the `tiferet` interface library:

```cmake
find_package(tiferet REQUIRED)
target_link_libraries(my_app PRIVATE tiferet)
```

Or use the umbrella header:

```cpp
#include <tiferet/tiferet.h>
```

When `TIFERET_YAML=OFF`, the framework has zero external dependencies and remains fully header-only.

## Quick Start: Calculator Example

A complete calculator application lives in `examples/calculator/`. It demonstrates the full vertical:

**1. Define YAML configurations:**

`configs/feature.yml` — feature workflows:
```yaml
features:
  calc:
    add:
      name: Add Number
      commands:
        - attribute_id: add_event
          name: Add a and b
          data_key: result
```

`configs/container.yml` — DI service bindings:
```yaml
attrs:
  add_event:
    module_path: calc.events
    class_name: AddEvent
```

`configs/error.yml` — error definitions:
```yaml
errors:
  division_by_zero:
    name: Division By Zero
    message:
      - lang: en_US
        text: 'Cannot divide by zero'
```

**2. Implement domain events:**

```cpp
class AddEvent : public Service, public ExecutableEvent {
public:
    std::string execute(const DataContext& data, const DataContext& params) override {
        // Read operands (params override data).
        double a = /* get "a" */;
        double b = /* get "b" */;
        return std::to_string(a + b);
    }
};
```

**3. Wire and run:**

```cpp
#include <tiferet/tiferet.h>

// Load repos from YAML.
FeatureYamlRepository features("configs/feature.yml");
ErrorYamlRepository errors("configs/error.yml");
DIYamlRepository di_configs("configs/container.yml");

// Register factories.
ServiceRegistry registry;
registry.register_service("calc.events::AddEvent",
    [](const ServiceParams&) { return std::make_unique<AddEvent>(); });

// Build DI container and app context.
DIContainer container(di_configs.list(), {"default"}, registry);
AppInterfaceContext app(features, errors, container);

// Execute a feature.
DataContext data = {{"a", "3"}, {"b", "4"}};
app.run("calc.add", data);
// data["result"] == "7"
```

Run the example:
```bash
ninja -C build
./build/examples/calculator/calculator
```

## Design Principles

- **Multiple inheritance is the key C++ advantage.** Aggregates and TransferObjects use MI mixins exactly like Python. Virtual `DomainObject` inheritance resolves the diamond.
- **`module_path`/`class_name` are plain strings.** Python uses `import_module + getattr`. C++ uses `ServiceRegistry` with `make_key(module_path, class_name)` → factory function lookup.
- **yaml-cpp is optional.** `TIFERET_YAML=OFF` keeps the framework header-only with zero external dependencies. The `TIFERET_HAS_YAML` preprocessor guard protects all YAML-dependent code.
- **`ExecutableEvent` is the pipeline interface.** Domain events that participate in feature pipelines implement `ExecutableEvent` alongside `Service`.
- **Package placement rule.** No new top-level directories. The structure is fixed: `assets`, `domain`, `events`, `interfaces`, `mappers`, `utils`, `repos`, `contexts`.

## Version History

| Version | Milestone | Description |
|---------|-----------|-------------|
| `v1.0.0a1` | Alpha 1 | Foundation: core type system, memory models, error handling, base classes |
| `v1.0.0a2` | Alpha 2 | MI-based mapper layer and first domain models (Error, Feature) |
| `v1.0.0a3` | Alpha 3 | Configuration utilities (YamlLoader) and App/DI domain models |
| `v1.0.0a4` | Alpha 4 | DI container, service registry, and feature pipeline execution |
| `v1.0.0a5` | Alpha 5 | YAML repositories, service interfaces, app runtime, full vertical integration |
| `v1.0.0b1` | Beta 1 | Public API surface, documentation, example applications |
