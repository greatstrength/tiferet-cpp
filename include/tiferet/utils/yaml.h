// *** tiferet/utils/yaml.h
// YAML file loading and saving utility for the Tiferet framework.
// Wraps yaml-cpp with structured error handling via TiferetException.
// Only available when TIFERET_HAS_YAML=1 (set by CMake when TIFERET_YAML is ON).

#ifndef TIFERET_UTILS_YAML_H
#define TIFERET_UTILS_YAML_H

#if TIFERET_HAS_YAML

#include <tiferet/assets/constants.h>
#include <tiferet/assets/exceptions.h>
#include <tiferet/events/settings.h>

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <string>

namespace tiferet {

// *** utils

// ** util: yaml_loader
/// Utility for loading and saving YAML files with structured error handling.
/// Wraps yaml-cpp's YAML::LoadFile and YAML::Emitter.
class YamlLoader {
public:

    // * init
    /// Construct a YamlLoader for the given file path.
    explicit YamlLoader(std::string path)
        : path_(std::move(path)) {}

    // * method: path
    /// Get the file path.
    const std::string& path() const { return path_; }

    // * method: load
    /// Load and parse the YAML file, returning the root node.
    /// Throws TiferetException with YAML_FILE_NOT_FOUND or YAML_LOAD_ERROR.
    YAML::Node load() const {

        // Verify the file exists.
        {
            std::ifstream probe(path_);
            if (!probe.good()) {
                DomainEvent::raise_error(
                    error_codes::YAML_FILE_NOT_FOUND,
                    path_.c_str());
            }
        }

        // Parse the YAML file.
        try {
            return YAML::LoadFile(path_);
        } catch (const YAML::Exception& e) {
            DomainEvent::raise_error(
                error_codes::YAML_LOAD_ERROR,
                e.what());
        }
    }

    // * method: load (static)
    /// One-shot static load: parse a YAML file and return the root node.
    static YAML::Node load(const std::string& path) {
        return YamlLoader(path).load();
    }

    // * method: save
    /// Serialize a YAML::Node to the file.
    /// Throws TiferetException with YAML_SAVE_ERROR on failure.
    void save(const YAML::Node& data) const {

        // Open the output file.
        try {
            std::ofstream out(path_);
            if (!out.good()) {
                DomainEvent::raise_error(
                    error_codes::YAML_SAVE_ERROR,
                    path_.c_str());
            }

            // Write the YAML content.
            out << data;
        } catch (const YAML::Exception& e) {
            DomainEvent::raise_error(
                error_codes::YAML_SAVE_ERROR,
                e.what());
        }
    }

    // * method: save (static)
    /// One-shot static save: serialize a YAML::Node to a file.
    static void save(const std::string& path, const YAML::Node& data) {
        YamlLoader(path).save(data);
    }

private:

    // * attribute: path_
    std::string path_;
};

} // namespace tiferet

#endif // TIFERET_HAS_YAML

#endif // TIFERET_UTILS_YAML_H
