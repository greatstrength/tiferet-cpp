// *** tiferet/repos/di.h
// YAML-backed ContainerService implementation.
// Loads ServiceConfiguration objects from a YAML configuration file (container.yml).
// Mirrors Python DIYamlRepository.
//
// Expected YAML structure:
//   attrs:
//     <service_id>:
//       module_path: <module>
//       class_name: <class>
//       params:                    # also accepts: parameters
//         <key>: <value>
//       dependencies:
//         - module_path: <module>
//           class_name: <class>
//           flag: <flag>
//           params:
//             <key>: <value>

#ifndef TIFERET_REPOS_DI_H
#define TIFERET_REPOS_DI_H

#if TIFERET_HAS_YAML

#include <tiferet/interfaces/container.h>
#include <tiferet/utils/yaml.h>
#include <tiferet/domain/di.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace tiferet {

// *** repos

// ** repo: di_yaml_repository
/// YAML-backed repository that loads ServiceConfiguration definitions.
/// Each top-level key under "attrs" becomes the service configuration ID.
class DIYamlRepository : public ContainerService {
public:

    // * init
    /// Construct and load all service configurations from the given YAML file.
    ///
    /// :param yaml_file: Path to the container configuration YAML file.
    explicit DIYamlRepository(const std::string& yaml_file) {
        load(yaml_file);
    }

    // * method: list
    const std::vector<ServiceConfiguration>& list() const override {
        return configs_;
    }

    // * method: size
    /// Return the number of loaded configurations.
    size_t size() const { return configs_.size(); }

private:

    // * method: load
    /// Parse the YAML file and populate the configuration list.
    void load(const std::string& yaml_file) {

        // Load the root YAML node.
        auto root = YamlLoader::load(yaml_file);

        // Navigate to the attrs section.
        auto attrs_node = root["attrs"];
        if (!attrs_node || !attrs_node.IsMap()) return;

        // Iterate over service configuration entries.
        for (auto it = attrs_node.begin(); it != attrs_node.end(); ++it) {

            std::string id = it->first.as<std::string>();
            auto cfg_node = it->second;
            if (!cfg_node.IsMap()) continue;

            // Read scalar fields.
            std::string name = cfg_node["name"].as<std::string>("");
            std::string module_path = cfg_node["module_path"].as<std::string>("");
            std::string class_name = cfg_node["class_name"].as<std::string>("");

            // Parse parameters (accepts params or parameters).
            auto parameters = parse_params(cfg_node);

            // Parse flagged dependencies.
            std::vector<FlaggedDependency> dependencies;
            if (cfg_node["dependencies"] && cfg_node["dependencies"].IsSequence()) {
                for (const auto& dep_node : cfg_node["dependencies"]) {
                    dependencies.push_back(parse_dependency(dep_node));
                }
            }

            configs_.emplace_back(
                std::move(id), std::move(name),
                std::move(module_path), std::move(class_name),
                std::move(parameters), std::move(dependencies));
        }
    }

    // * method: parse_params (static)
    /// Parse a parameter map from a YAML node (accepts params or parameters).
    static std::unordered_map<std::string, std::string> parse_params(
            const YAML::Node& node) {

        std::unordered_map<std::string, std::string> params;
        YAML::Node params_node;
        if (node["params"])           params_node = node["params"];
        else if (node["parameters"])  params_node = node["parameters"];

        if (params_node && params_node.IsMap()) {
            for (auto pit = params_node.begin(); pit != params_node.end(); ++pit) {
                params[pit->first.as<std::string>()] =
                    pit->second.as<std::string>();
            }
        }
        return params;
    }

    // * method: parse_dependency (static)
    /// Parse a single flagged dependency from a YAML node.
    static FlaggedDependency parse_dependency(const YAML::Node& node) {

        std::string module_path = node["module_path"].as<std::string>("");
        std::string class_name = node["class_name"].as<std::string>("");
        std::string flag = node["flag"].as<std::string>("");
        auto parameters = parse_params(node);

        return FlaggedDependency(
            std::move(module_path), std::move(class_name),
            std::move(flag), std::move(parameters));
    }

    // * attribute: configs_
    /// Loaded service configurations in insertion order.
    std::vector<ServiceConfiguration> configs_;
};

} // namespace tiferet

#endif // TIFERET_HAS_YAML

#endif // TIFERET_REPOS_DI_H
