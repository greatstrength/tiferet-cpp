// *** tiferet/domain/app.h
// App domain models for the Tiferet framework.
// AppServiceDependency defines a service binding for an app interface.
// AppInterface defines the application interface configuration.
// module_path/class_name are stored as strings; type resolution deferred to DI layer.

#ifndef TIFERET_DOMAIN_APP_H
#define TIFERET_DOMAIN_APP_H

#include <tiferet/domain/settings.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace tiferet {

// *** models

// ** model: app_service_dependency
/// A service dependency that defines a class binding for an app interface.
struct AppServiceDependency : virtual DomainObject {

    // * attribute: service_id
    /// The service identifier.
    std::string service_id;

    // * attribute: module_path
    /// The module path for the dependency class.
    std::string module_path;

    // * attribute: class_name
    /// The class name for the dependency.
    std::string class_name;

    // * attribute: parameters
    /// Configuration parameters for the dependency.
    std::unordered_map<std::string, std::string> parameters;

    // * init: default
    AppServiceDependency() = default;

    // * init: with fields
    AppServiceDependency(std::string service_id,
                         std::string module_path,
                         std::string class_name,
                         std::unordered_map<std::string, std::string> parameters = {})
        : service_id(std::move(service_id))
        , module_path(std::move(module_path))
        , class_name(std::move(class_name))
        , parameters(std::move(parameters)) {}

    // * method: validate
    bool validate() const noexcept override {
        return !service_id.empty() && !module_path.empty() && !class_name.empty();
    }
};

// ** model: app_interface
/// The application interface configuration.
struct AppInterface : virtual DomainObject {

    // * attribute: id
    /// The unique identifier for the interface.
    std::string id;

    // * attribute: name
    /// The display name.
    std::string name;

    // * attribute: description
    /// Optional description.
    std::string description;

    // * attribute: module_path
    /// The module path for the app context class.
    std::string module_path;

    // * attribute: class_name
    /// The class name for the app context.
    std::string class_name;

    // * attribute: logger_id
    /// The logger identifier for this interface.
    std::string logger_id = "default";

    // * attribute: flags
    /// Feature flags for this interface.
    std::vector<std::string> flags = {"default"};

    // * attribute: services
    /// The service dependencies.
    std::vector<AppServiceDependency> services;

    // * attribute: constants
    /// Application constants.
    std::unordered_map<std::string, std::string> constants;

    // * init: default
    AppInterface() = default;

    // * init: with fields
    AppInterface(std::string id,
                 std::string name,
                 std::string module_path = "",
                 std::string class_name = "",
                 std::string description = "",
                 std::string logger_id = "default",
                 std::vector<std::string> flags = {"default"},
                 std::vector<AppServiceDependency> services = {},
                 std::unordered_map<std::string, std::string> constants = {})
        : id(std::move(id))
        , name(std::move(name))
        , module_path(std::move(module_path))
        , class_name(std::move(class_name))
        , description(std::move(description))
        , logger_id(std::move(logger_id))
        , flags(std::move(flags))
        , services(std::move(services))
        , constants(std::move(constants)) {}

    // * method: validate
    bool validate() const noexcept override {
        return !id.empty() && !name.empty();
    }

    // * method: get_service
    /// Find a service dependency by service_id, or nullptr if not found.
    const AppServiceDependency* get_service(const std::string& service_id) const {
        for (const auto& dep : services) {
            if (dep.service_id == service_id) return &dep;
        }
        return nullptr;
    }

    // * method: get_service (mutable)
    AppServiceDependency* get_service(const std::string& service_id) {
        for (auto& dep : services) {
            if (dep.service_id == service_id) return &dep;
        }
        return nullptr;
    }
};

} // namespace tiferet

#endif // TIFERET_DOMAIN_APP_H
