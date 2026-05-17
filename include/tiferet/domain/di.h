// *** tiferet/domain/di.h
// Dependency injection domain models for the Tiferet framework.
// FlaggedDependency defines a flag-specific implementation override.
// ServiceConfiguration defines the DI wiring for a service.
// module_path/class_name are stored as strings; type resolution deferred to DI layer.

#ifndef TIFERET_DOMAIN_DI_H
#define TIFERET_DOMAIN_DI_H

#include <tiferet/domain/settings.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace tiferet {

// *** models

// ** model: flagged_dependency
/// A flag-specific implementation override for a service configuration.
struct FlaggedDependency : virtual DomainObject {

    // * attribute: module_path
    /// The module path for the dependency class.
    std::string module_path;

    // * attribute: class_name
    /// The class name for the dependency.
    std::string class_name;

    // * attribute: flag
    /// The flag that activates this dependency.
    std::string flag;

    // * attribute: parameters
    /// Configuration parameters for the dependency.
    std::unordered_map<std::string, std::string> parameters;

    // * init: default
    FlaggedDependency() = default;

    // * init: with fields
    FlaggedDependency(std::string module_path,
                      std::string class_name,
                      std::string flag,
                      std::unordered_map<std::string, std::string> parameters = {})
        : module_path(std::move(module_path))
        , class_name(std::move(class_name))
        , flag(std::move(flag))
        , parameters(std::move(parameters)) {}

    // * method: validate
    bool validate() const noexcept override {
        return !module_path.empty() && !class_name.empty() && !flag.empty();
    }
};

// ** model: service_configuration
/// A service configuration that defines dependency injection behavior.
struct ServiceConfiguration : virtual DomainObject {

    // * attribute: id
    /// The unique identifier for the service configuration.
    std::string id;

    // * attribute: name
    /// Optional display name.
    std::string name;

    // * attribute: module_path
    /// The default module path for the dependency class.
    std::string module_path;

    // * attribute: class_name
    /// The default class name for the dependency.
    std::string class_name;

    // * attribute: parameters
    /// Default configuration parameters.
    std::unordered_map<std::string, std::string> parameters;

    // * attribute: dependencies
    /// Flag-specific implementation overrides.
    std::vector<FlaggedDependency> dependencies;

    // * init: default
    ServiceConfiguration() = default;

    // * init: with fields
    ServiceConfiguration(std::string id,
                         std::string name = "",
                         std::string module_path = "",
                         std::string class_name = "",
                         std::unordered_map<std::string, std::string> parameters = {},
                         std::vector<FlaggedDependency> dependencies = {})
        : id(std::move(id))
        , name(std::move(name))
        , module_path(std::move(module_path))
        , class_name(std::move(class_name))
        , parameters(std::move(parameters))
        , dependencies(std::move(dependencies)) {}

    // * method: validate
    bool validate() const noexcept override {
        return !id.empty();
    }

    // * method: get_dependency
    /// Find the first flagged dependency matching any of the provided flags.
    /// Flags are checked in order (priority).
    template <typename... Flags>
    const FlaggedDependency* get_dependency(Flags&&... flags) const {
        // Pack flags into an initializer list for iteration.
        for (const auto& flag : {std::string(std::forward<Flags>(flags))...}) {
            for (const auto& dep : dependencies) {
                if (dep.flag == flag) return &dep;
            }
        }
        return nullptr;
    }

    // * method: get_dependency (single flag)
    const FlaggedDependency* get_dependency(const std::string& flag) const {
        for (const auto& dep : dependencies) {
            if (dep.flag == flag) return &dep;
        }
        return nullptr;
    }
};

} // namespace tiferet

#endif // TIFERET_DOMAIN_DI_H
