// *** tiferet/contexts/registry.h
// Compile-time + runtime service registry for the Tiferet framework.
// Maps string keys (module_path::class_name) to factory functions that
// produce concrete Service instances. This is the C++ equivalent of
// Python's import_module(path) + getattr(module, class_name).

#ifndef TIFERET_CONTEXTS_REGISTRY_H
#define TIFERET_CONTEXTS_REGISTRY_H

#include <tiferet/interfaces/settings.h>
#include <tiferet/events/settings.h>
#include <tiferet/assets/constants.h>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace tiferet {

// *** types

// ** type: service_params
/// Parameter map passed to service factories during construction.
using ServiceParams = std::unordered_map<std::string, std::string>;

// ** type: factory_fn
/// Factory signature: takes a parameter map, returns an owning Service pointer.
using FactoryFn = std::function<std::unique_ptr<Service>(const ServiceParams&)>;

// *** classes

// ** class: service_registry
/// A registry that maps string keys to factory functions for creating
/// Service instances. Provides the C++ equivalent of Python's dynamic
/// import_module + getattr pattern used in ServiceConfiguration.get_service_type().
///
/// Keys follow the format "module_path::class_name" to mirror the
/// module_path/class_name fields in ServiceConfiguration and FlaggedDependency.
///
/// Usage:
///   ServiceRegistry registry;
///   registry.register_service("calc.events::AddNumber", [](const ServiceParams&) {
///       return std::make_unique<AddNumberEvent>();
///   });
///   auto svc = registry.create_service("calc.events::AddNumber", {});
class ServiceRegistry {
public:

    // * init
    ServiceRegistry() = default;

    // Non-copyable, movable.
    ServiceRegistry(const ServiceRegistry&) = delete;
    ServiceRegistry& operator=(const ServiceRegistry&) = delete;
    ServiceRegistry(ServiceRegistry&&) = default;
    ServiceRegistry& operator=(ServiceRegistry&&) = default;

    // * method: make_key (static)
    /// Build a registry key from module_path and class_name.
    /// Format: "module_path::class_name".
    static std::string make_key(const std::string& module_path,
                                const std::string& class_name) {
        return module_path + "::" + class_name;
    }

    // * method: register_service
    /// Register a factory function under the given key.
    /// Overwrites any existing registration for the same key.
    void register_service(const std::string& key, FactoryFn factory) {
        factories_[key] = std::move(factory);
    }

    // * method: create_service
    /// Create a Service instance by key, passing the parameter map to the factory.
    /// Raises SERVICE_NOT_REGISTERED if the key is not found.
    std::unique_ptr<Service> create_service(const std::string& key,
                                            const ServiceParams& params = {}) const {

        // Look up the factory for the given key.
        auto it = factories_.find(key);

        // Raise an error if the key is not registered.
        if (it == factories_.end()) {
            DomainEvent::raise_error(
                error_codes::SERVICE_NOT_REGISTERED,
                key.c_str()
            );
        }

        // Invoke the factory with the provided parameters.
        return it->second(params);
    }

    // * method: has
    /// Check whether a key is registered.
    bool has(const std::string& key) const {
        return factories_.count(key) > 0;
    }

    // * method: size
    /// Return the number of registered factories.
    size_t size() const { return factories_.size(); }

private:

    // * attribute: factories_
    /// The registry storage mapping keys to factory functions.
    std::unordered_map<std::string, FactoryFn> factories_;
};

// *** macros

// ** macro: TIFERET_REGISTER_SERVICE
/// Static registration macro. Registers a default factory that constructs
/// Type with its default constructor (ignoring params). For factories that
/// need params, use registry.register_service() directly.
///
/// Usage (at file scope):
///   TIFERET_REGISTER_SERVICE(registry, "calc.events::AddNumber", AddNumberEvent)
#define TIFERET_REGISTER_SERVICE(registry, key, Type) \
    do { \
        (registry).register_service((key), \
            [](const ::tiferet::ServiceParams&) -> std::unique_ptr<::tiferet::Service> { \
                return std::make_unique<Type>(); \
            }); \
    } while (0)

} // namespace tiferet

#endif // TIFERET_CONTEXTS_REGISTRY_H
