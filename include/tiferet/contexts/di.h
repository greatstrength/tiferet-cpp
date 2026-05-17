// *** tiferet/contexts/di.h
// DI container for the Tiferet framework.
// Resolves ServiceConfiguration objects into live Service instances
// using the ServiceRegistry. Mirrors Python DIContext.
//
// For each configuration, the container selects the appropriate
// module_path/class_name (flagged dependency or default), builds
// the registry key, creates the service, and caches it.

#ifndef TIFERET_CONTEXTS_DI_H
#define TIFERET_CONTEXTS_DI_H

#include <tiferet/contexts/registry.h>
#include <tiferet/domain/di.h>
#include <tiferet/assets/constants.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace tiferet {

// *** contexts

// ** context: di_container
/// A dependency injection container that resolves ServiceConfiguration
/// entries into concrete Service instances using the ServiceRegistry.
///
/// Mirrors Python DIContext.build_service_provider() → get_dependency().
///
/// Usage:
///   ServiceRegistry registry;
///   // ... register factories ...
///   DIContainer container(configs, {"default"}, registry);
///   Service* svc = container.get_service("my_service");
class DIContainer {
public:

    // * init
    /// Construct and resolve all services from the given configurations.
    ///
    /// :param configs: The service configurations to resolve.
    /// :param flags: Feature flags for dependency selection (priority order).
    /// :param registry: The service registry containing factory functions.
    DIContainer(const std::vector<ServiceConfiguration>& configs,
                const std::vector<std::string>& flags,
                const ServiceRegistry& registry) {

        // Resolve each configuration into a live service.
        for (const auto& config : configs) {
            resolve(config, flags, registry);
        }
    }

    // Non-copyable, movable.
    DIContainer(const DIContainer&) = delete;
    DIContainer& operator=(const DIContainer&) = delete;
    DIContainer(DIContainer&&) = default;
    DIContainer& operator=(DIContainer&&) = default;

    // * method: get_service
    /// Retrieve a resolved service by its configuration ID.
    /// Returns nullptr if the ID was not resolved.
    Service* get_service(const std::string& id) const {

        // Look up the service by its configuration ID.
        auto it = services_.find(id);

        // Return the raw pointer, or nullptr if not found.
        if (it == services_.end()) return nullptr;
        return it->second.get();
    }

    // * method: has
    /// Check whether a service has been resolved for the given ID.
    bool has(const std::string& id) const {
        return services_.count(id) > 0;
    }

    // * method: size
    /// Return the number of resolved services.
    size_t size() const { return services_.size(); }

private:

    // * method: resolve
    /// Resolve a single ServiceConfiguration into a Service instance.
    /// Checks flagged dependencies first (in flag priority order),
    /// then falls back to the configuration's default module_path/class_name.
    void resolve(const ServiceConfiguration& config,
                 const std::vector<std::string>& flags,
                 const ServiceRegistry& registry) {

        std::string module_path;
        std::string class_name;
        ServiceParams params;

        // Check flagged dependencies first (mirrors Python get_service_type).
        for (const auto& flag : flags) {
            const auto* dep = config.get_dependency(flag);
            if (dep) {
                module_path = dep->module_path;
                class_name = dep->class_name;
                params = dep->parameters;
                break;
            }
        }

        // Fall back to the configuration's default type.
        if (module_path.empty() && class_name.empty()) {
            module_path = config.module_path;
            class_name = config.class_name;
            params = config.parameters;
        }

        // If no type is available, raise an error.
        if (module_path.empty() || class_name.empty()) {
            DomainEvent::raise_error(
                error_codes::SERVICE_RESOLUTION_FAILED,
                config.id.c_str()
            );
        }

        // Build the registry key and create the service.
        auto key = ServiceRegistry::make_key(module_path, class_name);
        auto service = registry.create_service(key, params);

        // Cache the resolved service.
        services_[config.id] = std::move(service);
    }

    // * attribute: services_
    /// Resolved service instances, keyed by configuration ID.
    std::unordered_map<std::string, std::unique_ptr<Service>> services_;
};

} // namespace tiferet

#endif // TIFERET_CONTEXTS_DI_H
