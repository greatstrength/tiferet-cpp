// *** tiferet/repos/app.h
// YAML-backed AppService implementation.
// Loads AppInterface objects from a YAML configuration file (app.yml).
// Mirrors Python AppYamlRepository.
//
// Expected YAML structure:
//   interfaces:
//     <interface_id>:
//       name: <name>
//       description: <description>
//       module_path: <module>
//       class_name: <class>
//       flags:
//         - <flag>
//       attrs:                      # service dependencies
//         <service_id>:
//           module_path: <module>
//           class_name: <class>
//           params:
//             <key>: <value>

#ifndef TIFERET_REPOS_APP_H
#define TIFERET_REPOS_APP_H

#if TIFERET_HAS_YAML

#include <tiferet/interfaces/app.h>
#include <tiferet/utils/yaml.h>
#include <tiferet/domain/app.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace tiferet {

// *** repos

// ** repo: app_yaml_repository
/// YAML-backed repository that loads AppInterface definitions from a YAML file.
/// Each top-level key under "interfaces" becomes the interface ID.
class AppYamlRepository : public AppService {
public:

    // * init
    /// Construct and load all app interfaces from the given YAML file.
    ///
    /// :param yaml_file: Path to the app configuration YAML file.
    explicit AppYamlRepository(const std::string& yaml_file) {
        load(yaml_file);
    }

    // * method: get
    const AppInterface* get(const std::string& id) const override {

        // Look up by interface identifier.
        auto it = interfaces_.find(id);

        // Return pointer or nullptr.
        return (it != interfaces_.end()) ? &it->second : nullptr;
    }

    // * method: exists
    bool exists(const std::string& id) const override {
        return interfaces_.count(id) > 0;
    }

    // * method: size
    /// Return the number of loaded interfaces.
    size_t size() const { return interfaces_.size(); }

private:

    // * method: load
    /// Parse the YAML file and populate the interface map.
    void load(const std::string& yaml_file) {

        // Load the root YAML node.
        auto root = YamlLoader::load(yaml_file);

        // Navigate to the interfaces section.
        auto ifaces_node = root["interfaces"];
        if (!ifaces_node || !ifaces_node.IsMap()) return;

        // Iterate over interface entries.
        for (auto it = ifaces_node.begin(); it != ifaces_node.end(); ++it) {

            std::string id = it->first.as<std::string>();
            auto iface_node = it->second;
            if (!iface_node.IsMap()) continue;

            // Parse the interface from the YAML node.
            parse_interface(iface_node, id);
        }
    }

    // * method: parse_interface
    /// Parse a single app interface node and store it in the map.
    void parse_interface(const YAML::Node& node, const std::string& id) {

        // Read scalar fields.
        std::string name = node["name"].as<std::string>("");
        std::string description = node["description"].as<std::string>("");
        std::string module_path = node["module_path"].as<std::string>("");
        std::string class_name = node["class_name"].as<std::string>("");
        std::string logger_id = node["logger_id"].as<std::string>("default");

        // Parse flags (defaults to ["default"]).
        std::vector<std::string> flags = {"default"};
        if (node["flags"] && node["flags"].IsSequence()) {
            flags.clear();
            for (const auto& flag_node : node["flags"]) {
                flags.push_back(flag_node.as<std::string>());
            }
        }

        // Parse service dependencies from attrs.
        std::vector<AppServiceDependency> services;
        auto attrs_node = node["attrs"];
        if (attrs_node && attrs_node.IsMap()) {
            for (auto sit = attrs_node.begin(); sit != attrs_node.end(); ++sit) {
                std::string service_id = sit->first.as<std::string>();
                auto svc_node = sit->second;
                if (!svc_node.IsMap()) continue;

                services.push_back(parse_service_dep(svc_node, service_id));
            }
        }

        // Parse constants.
        std::unordered_map<std::string, std::string> constants;
        if (node["constants"] && node["constants"].IsMap()) {
            for (auto cit = node["constants"].begin();
                 cit != node["constants"].end(); ++cit) {
                constants[cit->first.as<std::string>()] =
                    cit->second.as<std::string>();
            }
        }

        interfaces_.emplace(id, AppInterface(
            id, std::move(name), std::move(module_path),
            std::move(class_name), std::move(description),
            std::move(logger_id), std::move(flags),
            std::move(services), std::move(constants)));
    }

    // * method: parse_service_dep (static)
    /// Parse a single service dependency from a YAML node.
    static AppServiceDependency parse_service_dep(const YAML::Node& node,
                                                  const std::string& service_id) {

        std::string module_path = node["module_path"].as<std::string>("");
        std::string class_name = node["class_name"].as<std::string>("");

        // Parse parameters (accepts params or parameters).
        std::unordered_map<std::string, std::string> parameters;
        YAML::Node params_node;
        if (node["params"])           params_node = node["params"];
        else if (node["parameters"])  params_node = node["parameters"];

        if (params_node && params_node.IsMap()) {
            for (auto pit = params_node.begin(); pit != params_node.end(); ++pit) {
                parameters[pit->first.as<std::string>()] =
                    pit->second.as<std::string>();
            }
        }

        return AppServiceDependency(
            service_id, std::move(module_path),
            std::move(class_name), std::move(parameters));
    }

    // * attribute: interfaces_
    /// Loaded app interfaces, keyed by identifier.
    std::unordered_map<std::string, AppInterface> interfaces_;
};

} // namespace tiferet

#endif // TIFERET_HAS_YAML

#endif // TIFERET_REPOS_APP_H
