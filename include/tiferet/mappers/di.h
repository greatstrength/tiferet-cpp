// *** tiferet/mappers/di.h
// DI mapper classes for the Tiferet framework.
// ServiceConfigurationAggregate provides mutable DI operations via MI.
// ServiceConfigurationYamlObject provides serialization via MI.

#ifndef TIFERET_MAPPERS_DI_H
#define TIFERET_MAPPERS_DI_H

#include <tiferet/domain/di.h>
#include <tiferet/mappers/settings.h>

#include <algorithm>
#include <string>
#include <vector>

namespace tiferet {

// *** mappers

// ** mapper: flagged_dependency_aggregate
/// Mutable FlaggedDependency aggregate.
struct FlaggedDependencyAggregate : FlaggedDependency, Aggregate {

    // * init: default
    FlaggedDependencyAggregate() = default;

    // * init: with fields
    FlaggedDependencyAggregate(std::string module_path,
                               std::string class_name,
                               std::string flag,
                               std::unordered_map<std::string, std::string> parameters = {})
        : FlaggedDependency(std::move(module_path), std::move(class_name),
                            std::move(flag), std::move(parameters)) {}

    // * method: set_attribute
    void set_attribute(const std::string& attribute, const std::string& value) override {
        if (attribute == "module_path") { module_path = value; return; }
        if (attribute == "class_name") { class_name = value; return; }
        if (attribute == "flag") { flag = value; return; }
        Aggregate::set_attribute(attribute, value);
    }

    // * method: set_parameters
    /// Merge parameters. Empty-value keys are removed.
    void set_parameters(const std::unordered_map<std::string, std::string>& new_params) {
        for (const auto& [key, value] : new_params) {
            if (value.empty()) {
                parameters.erase(key);
            } else {
                parameters[key] = value;
            }
        }
    }
};

// ** mapper: flagged_dependency_yaml_object
/// YAML transfer object for FlaggedDependency.
struct FlaggedDependencyYamlObject : FlaggedDependency, TransferObject {

    // * init: default
    FlaggedDependencyYamlObject() = default;

    // * init: with fields
    FlaggedDependencyYamlObject(std::string module_path,
                                std::string class_name,
                                std::string flag = "",
                                std::unordered_map<std::string, std::string> parameters = {})
        : FlaggedDependency(std::move(module_path), std::move(class_name),
                            std::move(flag), std::move(parameters)) {}

    // * method: to_map
    FieldMap to_map(const std::string& role = "") const override {
        FieldMap m;
        if (role != "to_data" && !flag.empty()) m["flag"] = flag;
        if (!module_path.empty()) m["module_path"] = module_path;
        if (!class_name.empty()) m["class_name"] = class_name;
        return m;
    }

    // * method: map
    /// Map to a FlaggedDependency, optionally overriding the flag.
    FlaggedDependency map(const std::string& flag_override = "") const {
        return FlaggedDependency{
            module_path, class_name,
            flag_override.empty() ? flag : flag_override,
            parameters};
    }

    // * method: from_model (static)
    static FlaggedDependencyYamlObject from_model(const FlaggedDependency& dep) {
        return FlaggedDependencyYamlObject{
            dep.module_path, dep.class_name, dep.flag, dep.parameters};
    }
};

// ** mapper: service_configuration_aggregate
/// Mutable ServiceConfiguration aggregate.
struct ServiceConfigurationAggregate : ServiceConfiguration, Aggregate {

    // * init: default
    ServiceConfigurationAggregate() = default;

    // * init: with fields
    ServiceConfigurationAggregate(std::string id,
                                   std::string name = "",
                                   std::string module_path = "",
                                   std::string class_name = "",
                                   std::unordered_map<std::string, std::string> parameters = {},
                                   std::vector<FlaggedDependency> dependencies = {})
        : ServiceConfiguration(std::move(id), std::move(name),
                               std::move(module_path), std::move(class_name),
                               std::move(parameters), std::move(dependencies)) {}

    // * method: set_attribute
    void set_attribute(const std::string& attribute, const std::string& value) override {
        if (attribute == "name") { name = value; return; }
        if (attribute == "module_path") { module_path = value; return; }
        if (attribute == "class_name") { class_name = value; return; }
        Aggregate::set_attribute(attribute, value);
    }

    // * method: set_default_type
    /// Update the default type and parameters.
    void set_default_type(const std::string& mod_path,
                          const std::string& cls_name,
                          const std::unordered_map<std::string, std::string>& params = {}) {
        module_path = mod_path;
        class_name = cls_name;
        parameters = params;
    }

    // * method: set_dependency
    /// Set or update a flagged dependency.
    void set_dependency(const std::string& dep_flag,
                        const std::string& mod_path,
                        const std::string& cls_name,
                        const std::unordered_map<std::string, std::string>& params = {}) {

        // Update existing dependency in place if found.
        for (auto& dep : dependencies) {
            if (dep.flag == dep_flag) {
                dep.module_path = mod_path;
                dep.class_name = cls_name;
                for (const auto& [k, v] : params) {
                    if (v.empty()) dep.parameters.erase(k);
                    else dep.parameters[k] = v;
                }
                return;
            }
        }

        // Append new dependency.
        dependencies.emplace_back(mod_path, cls_name, dep_flag, params);
    }

    // * method: remove_dependency
    /// Remove a flagged dependency by flag.
    void remove_dependency(const std::string& dep_flag) {
        dependencies.erase(
            std::remove_if(dependencies.begin(), dependencies.end(),
                [&](const FlaggedDependency& d) { return d.flag == dep_flag; }),
            dependencies.end());
    }
};

// ** mapper: service_configuration_yaml_object
/// YAML transfer object for ServiceConfiguration.
struct ServiceConfigurationYamlObject : ServiceConfiguration, TransferObject {

    // * init: default
    ServiceConfigurationYamlObject() = default;

    // * init: with fields
    ServiceConfigurationYamlObject(std::string id,
                                    std::string name = "",
                                    std::string module_path = "",
                                    std::string class_name = "",
                                    std::unordered_map<std::string, std::string> parameters = {},
                                    std::vector<FlaggedDependency> dependencies = {})
        : ServiceConfiguration(std::move(id), std::move(name),
                               std::move(module_path), std::move(class_name),
                               std::move(parameters), std::move(dependencies)) {}

    // * method: to_map
    FieldMap to_map(const std::string& role = "") const override {
        FieldMap m;
        if (role != "to_data" && !id.empty()) m["id"] = id;
        if (!name.empty()) m["name"] = name;
        if (!module_path.empty()) m["module_path"] = module_path;
        if (!class_name.empty()) m["class_name"] = class_name;
        return m;
    }

    // * method: map
    ServiceConfigurationAggregate map() const {
        return ServiceConfigurationAggregate{
            id, name, module_path, class_name,
            parameters, dependencies};
    }

    // * method: from_model (static)
    static ServiceConfigurationYamlObject from_model(const ServiceConfiguration& cfg) {
        return ServiceConfigurationYamlObject{
            cfg.id, cfg.name, cfg.module_path, cfg.class_name,
            cfg.parameters, cfg.dependencies};
    }
};

} // namespace tiferet

#endif // TIFERET_MAPPERS_DI_H
