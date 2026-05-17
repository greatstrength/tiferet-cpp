// *** tiferet/repos/feature.h
// YAML-backed FeatureService implementation.
// Loads Feature objects from a YAML configuration file (feature.yml).
// Mirrors Python FeatureYamlRepository.
//
// Expected YAML structure:
//   features:
//     <group_id>:
//       <feature_key>:
//         name: <name>
//         description: <description>
//         commands:                          # also accepts: handlers, steps
//           - attribute_id: <service_id>     # also accepts: service_id
//             name: <step_name>
//             params:                        # also accepts: parameters
//               <key>: <value>
//             data_key: <key>
//             pass_on_error: true/false

#ifndef TIFERET_REPOS_FEATURE_H
#define TIFERET_REPOS_FEATURE_H

#if TIFERET_HAS_YAML

#include <tiferet/interfaces/feature.h>
#include <tiferet/utils/yaml.h>
#include <tiferet/domain/feature.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace tiferet {

// *** repos

// ** repo: feature_yaml_repository
/// YAML-backed repository that loads Feature definitions from a YAML file.
/// Parses the nested group/key structure and derives composite identifiers.
class FeatureYamlRepository : public FeatureService {
public:

    // * init
    /// Construct and load all features from the given YAML file.
    ///
    /// :param yaml_file: Path to the feature configuration YAML file.
    explicit FeatureYamlRepository(const std::string& yaml_file) {
        load(yaml_file);
    }

    // * method: get
    const Feature* get(const std::string& id) const override {

        // Look up by composite identifier.
        auto it = features_.find(id);

        // Return pointer or nullptr.
        return (it != features_.end()) ? &it->second : nullptr;
    }

    // * method: exists
    bool exists(const std::string& id) const override {
        return features_.count(id) > 0;
    }

    // * method: size
    /// Return the number of loaded features.
    size_t size() const { return features_.size(); }

private:

    // * method: load
    /// Parse the YAML file and populate the feature map.
    void load(const std::string& yaml_file) {

        // Load the root YAML node.
        auto root = YamlLoader::load(yaml_file);

        // Navigate to the features section.
        auto features_node = root["features"];
        if (!features_node || !features_node.IsMap()) return;

        // Iterate over feature groups.
        for (auto group_it = features_node.begin();
             group_it != features_node.end(); ++group_it) {

            std::string group_id = group_it->first.as<std::string>();
            auto group_node = group_it->second;
            if (!group_node.IsMap()) continue;

            // Iterate over features within the group.
            for (auto feat_it = group_node.begin();
                 feat_it != group_node.end(); ++feat_it) {

                std::string feature_key = feat_it->first.as<std::string>();
                auto feat_node = feat_it->second;
                if (!feat_node.IsMap()) continue;

                // Parse the feature from the YAML node.
                parse_feature(feat_node, group_id, feature_key);
            }
        }
    }

    // * method: parse_feature
    /// Parse a single feature node and store it in the map.
    void parse_feature(const YAML::Node& node,
                       const std::string& group_id,
                       const std::string& feature_key) {

        // Read scalar fields.
        std::string name = node["name"].as<std::string>("");
        std::string description = node["description"].as<std::string>("");

        // Parse the step list (accepts commands, handlers, or steps).
        std::vector<FeatureEvent> steps;
        YAML::Node steps_node;
        if (node["commands"])      steps_node = node["commands"];
        else if (node["handlers"]) steps_node = node["handlers"];
        else if (node["steps"])    steps_node = node["steps"];

        if (steps_node && steps_node.IsSequence()) {
            for (const auto& cmd_node : steps_node) {
                steps.push_back(parse_step(cmd_node));
            }
        }

        // Derive composite keys.
        std::string id;
        std::string gid = group_id;
        std::string fk = feature_key;
        Feature::derive_keys(id, gid, fk, name, description);

        // Store the feature.
        features_.emplace(id, Feature(
            id, name, gid, fk, std::move(steps), description));
    }

    // * method: parse_step (static)
    /// Parse a single feature event step from a YAML node.
    static FeatureEvent parse_step(const YAML::Node& node) {

        // Read step name.
        std::string name = node["name"].as<std::string>("");

        // Read service_id (accepts attribute_id or service_id).
        std::string service_id;
        if (node["attribute_id"])      service_id = node["attribute_id"].as<std::string>();
        else if (node["service_id"])   service_id = node["service_id"].as<std::string>();

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

        // Read optional fields.
        std::string data_key = node["data_key"].as<std::string>("");
        bool pass_on_error = node["pass_on_error"].as<bool>(false);
        std::string condition = node["condition"].as<std::string>("");

        return FeatureEvent(
            std::move(name), std::move(service_id),
            std::move(parameters), std::move(data_key),
            pass_on_error, std::move(condition));
    }

    // * attribute: features_
    /// Loaded features, keyed by composite identifier.
    std::unordered_map<std::string, Feature> features_;
};

} // namespace tiferet

#endif // TIFERET_HAS_YAML

#endif // TIFERET_REPOS_FEATURE_H
