// *** tiferet/mappers/app.h
// App mapper classes for the Tiferet framework.
// AppInterfaceAggregate provides mutable operations via MI (AppInterface + Aggregate).
// AppInterfaceYamlObject provides serialization via MI (AppInterface + TransferObject).

#ifndef TIFERET_MAPPERS_APP_H
#define TIFERET_MAPPERS_APP_H

#include <tiferet/domain/app.h>
#include <tiferet/mappers/settings.h>

#include <algorithm>
#include <string>
#include <vector>

namespace tiferet {

// *** mappers

// ** mapper: app_interface_aggregate
/// Mutable AppInterface aggregate.
struct AppInterfaceAggregate : AppInterface, Aggregate {

    // * init: default
    AppInterfaceAggregate() = default;

    // * init: with fields
    AppInterfaceAggregate(std::string id,
                          std::string name,
                          std::string module_path = "",
                          std::string class_name = "",
                          std::string description = "",
                          std::string logger_id = "default",
                          std::vector<std::string> flags = {"default"},
                          std::vector<AppServiceDependency> services = {},
                          std::unordered_map<std::string, std::string> constants = {})
        : AppInterface(std::move(id), std::move(name), std::move(module_path),
                       std::move(class_name), std::move(description),
                       std::move(logger_id), std::move(flags),
                       std::move(services), std::move(constants)) {}

    // * method: set_attribute
    void set_attribute(const std::string& attribute, const std::string& value) override {
        if (attribute == "name") { name = value; return; }
        if (attribute == "description") { description = value; return; }
        if (attribute == "module_path") { module_path = value; return; }
        if (attribute == "class_name") { class_name = value; return; }
        if (attribute == "logger_id") { logger_id = value; return; }

        // Unknown attribute: delegate to base (raises error).
        Aggregate::set_attribute(attribute, value);
    }

    // * method: add_service
    /// Add a service dependency.
    void add_service(AppServiceDependency dep) {
        services.push_back(std::move(dep));
    }

    // * method: remove_service
    /// Remove a service dependency by service_id. Returns true if found.
    bool remove_service(const std::string& service_id) {
        auto it = std::find_if(services.begin(), services.end(),
            [&](const AppServiceDependency& d) { return d.service_id == service_id; });
        if (it == services.end()) return false;
        services.erase(it);
        return true;
    }

    // * method: set_constants
    /// Merge constants. Empty-value keys are removed.
    void set_constants(const std::unordered_map<std::string, std::string>& new_constants) {
        for (const auto& [key, value] : new_constants) {
            if (value.empty()) {
                constants.erase(key);
            } else {
                constants[key] = value;
            }
        }
    }
};

// ** mapper: app_service_dependency_yaml_object
/// YAML transfer object for AppServiceDependency.
struct AppServiceDependencyYamlObject : AppServiceDependency, TransferObject {

    // * init: default
    AppServiceDependencyYamlObject() = default;

    // * init: with fields
    AppServiceDependencyYamlObject(std::string service_id,
                                   std::string module_path,
                                   std::string class_name,
                                   std::unordered_map<std::string, std::string> parameters = {})
        : AppServiceDependency(std::move(service_id), std::move(module_path),
                               std::move(class_name), std::move(parameters)) {}

    // * method: to_map
    FieldMap to_map(const std::string& role = "") const override {
        FieldMap m;
        if (role != "to_data" && !service_id.empty()) m["service_id"] = service_id;
        if (!module_path.empty()) m["module_path"] = module_path;
        if (!class_name.empty()) m["class_name"] = class_name;
        return m;
    }

    // * method: map
    AppServiceDependency map(const std::string& sid = "") const {
        return AppServiceDependency{
            sid.empty() ? service_id : sid,
            module_path, class_name, parameters};
    }

    // * method: from_model (static)
    static AppServiceDependencyYamlObject from_model(const AppServiceDependency& dep) {
        return AppServiceDependencyYamlObject{
            dep.service_id, dep.module_path, dep.class_name, dep.parameters};
    }
};

// ** mapper: app_interface_yaml_object
/// YAML transfer object for AppInterface.
struct AppInterfaceYamlObject : AppInterface, TransferObject {

    // * init: default
    AppInterfaceYamlObject() = default;

    // * init: with fields
    AppInterfaceYamlObject(std::string id,
                           std::string name,
                           std::string module_path = "",
                           std::string class_name = "",
                           std::string description = "",
                           std::string logger_id = "default",
                           std::vector<std::string> flags = {"default"},
                           std::vector<AppServiceDependency> services = {},
                           std::unordered_map<std::string, std::string> constants = {})
        : AppInterface(std::move(id), std::move(name), std::move(module_path),
                       std::move(class_name), std::move(description),
                       std::move(logger_id), std::move(flags),
                       std::move(services), std::move(constants)) {}

    // * method: to_map
    FieldMap to_map(const std::string& role = "") const override {
        FieldMap m;
        if (role == "to_data") {
            // Exclude id for YAML storage.
            if (!name.empty()) m["name"] = name;
            if (!description.empty()) m["description"] = description;
            if (!module_path.empty()) m["module_path"] = module_path;
            if (!class_name.empty()) m["class_name"] = class_name;
            if (!logger_id.empty()) m["logger_id"] = logger_id;
        } else {
            if (!id.empty()) m["id"] = id;
            if (!name.empty()) m["name"] = name;
            if (!description.empty()) m["description"] = description;
            if (!module_path.empty()) m["module_path"] = module_path;
            if (!class_name.empty()) m["class_name"] = class_name;
            if (!logger_id.empty()) m["logger_id"] = logger_id;
        }
        return m;
    }

    // * method: map
    AppInterfaceAggregate map() const {
        return AppInterfaceAggregate{
            id, name, module_path, class_name, description,
            logger_id, flags, services, constants};
    }

    // * method: from_model (static)
    static AppInterfaceYamlObject from_model(const AppInterface& iface) {
        return AppInterfaceYamlObject{
            iface.id, iface.name, iface.module_path, iface.class_name,
            iface.description, iface.logger_id, iface.flags,
            iface.services, iface.constants};
    }
};

} // namespace tiferet

#endif // TIFERET_MAPPERS_APP_H
