// *** tiferet/repos/error.h
// YAML-backed ErrorService implementation.
// Loads Error objects from a YAML configuration file (error.yml).
// Mirrors Python ErrorYamlRepository.
//
// Expected YAML structure:
//   errors:
//     <error_id>:
//       name: <name>
//       message:
//         - lang: <lang>
//           text: <text>

#ifndef TIFERET_REPOS_ERROR_H
#define TIFERET_REPOS_ERROR_H

#if TIFERET_HAS_YAML

#include <tiferet/interfaces/error.h>
#include <tiferet/utils/yaml.h>
#include <tiferet/domain/error.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace tiferet {

// *** repos

// ** repo: error_yaml_repository
/// YAML-backed repository that loads Error definitions from a YAML file.
/// Supports lookup by both the YAML key (id) and the derived error_code.
class ErrorYamlRepository : public ErrorService {
public:

    // * init
    /// Construct and load all errors from the given YAML file.
    ///
    /// :param yaml_file: Path to the error configuration YAML file.
    explicit ErrorYamlRepository(const std::string& yaml_file) {
        load(yaml_file);
    }

    // * method: get
    const Error* get(const std::string& id_or_code) const override {

        // Try direct id lookup first.
        auto it = errors_.find(id_or_code);
        if (it != errors_.end()) return &it->second;

        // Try error_code index.
        auto code_it = code_index_.find(id_or_code);
        if (code_it != code_index_.end()) {
            it = errors_.find(code_it->second);
            if (it != errors_.end()) return &it->second;
        }

        return nullptr;
    }

    // * method: exists
    bool exists(const std::string& id_or_code) const override {

        // Check id map.
        if (errors_.count(id_or_code) > 0) return true;

        // Check error_code index.
        return code_index_.count(id_or_code) > 0;
    }

    // * method: size
    /// Return the number of loaded errors.
    size_t size() const { return errors_.size(); }

private:

    // * method: load
    /// Parse the YAML file and populate the error map.
    void load(const std::string& yaml_file) {

        // Load the root YAML node.
        auto root = YamlLoader::load(yaml_file);

        // Navigate to the errors section.
        auto errors_node = root["errors"];
        if (!errors_node || !errors_node.IsMap()) return;

        // Iterate over error entries.
        for (auto it = errors_node.begin(); it != errors_node.end(); ++it) {

            std::string id = it->first.as<std::string>();
            auto err_node = it->second;
            if (!err_node.IsMap()) continue;

            // Read scalar fields.
            std::string name = err_node["name"].as<std::string>("");
            std::string description = err_node["description"].as<std::string>("");

            // Parse localized messages.
            std::vector<ErrorMessage> messages;
            if (err_node["message"] && err_node["message"].IsSequence()) {
                for (const auto& msg_node : err_node["message"]) {
                    std::string lang = msg_node["lang"].as<std::string>("");
                    std::string text = msg_node["text"].as<std::string>("");
                    messages.emplace_back(std::move(lang), std::move(text));
                }
            }

            // Construct the Error (error_code derived from id in constructor).
            auto result = errors_.emplace(
                id, Error(id, name, std::move(messages), description));

            // Index by error_code for runtime lookup.
            const auto& error_code = result.first->second.error_code;
            if (!error_code.empty()) {
                code_index_[error_code] = id;
            }
        }
    }

    // * attribute: errors_
    /// Loaded errors, keyed by id.
    std::unordered_map<std::string, Error> errors_;

    // * attribute: code_index_
    /// Maps uppercase error_code to id for dual-key lookup.
    std::unordered_map<std::string, std::string> code_index_;
};

} // namespace tiferet

#endif // TIFERET_HAS_YAML

#endif // TIFERET_REPOS_ERROR_H
