// *** tiferet/mappers/feature.h
// Feature mapper classes for the Tiferet framework.
// FeatureEventAggregate and FeatureAggregate provide mutable operations via MI.
// FeatureEventYamlObject and FeatureYamlObject provide serialization via MI.

#ifndef TIFERET_MAPPERS_FEATURE_H
#define TIFERET_MAPPERS_FEATURE_H

#include <tiferet/domain/feature.h>
#include <tiferet/mappers/settings.h>

#include <algorithm>
#include <string>
#include <vector>

namespace tiferet {

// *** mappers

// ** mapper: feature_event_aggregate
/// Mutable FeatureEvent aggregate.
/// Uses MI: inherits data from FeatureEvent, mutation from Aggregate.
struct FeatureEventAggregate : FeatureEvent, Aggregate {

    // * init: default
    FeatureEventAggregate() = default;

    // * init: with fields
    FeatureEventAggregate(std::string name,
                          std::string service_id,
                          std::unordered_map<std::string, std::string> parameters = {},
                          std::string data_key = "",
                          bool pass_on_error = false,
                          std::string condition = "")
        : FeatureEvent(std::move(name), std::move(service_id),
                       std::move(parameters), std::move(data_key),
                       pass_on_error, std::move(condition)) {}

    // * method: set_attribute
    /// Dispatch to known FeatureEvent fields or raise INVALID_MODEL_ATTRIBUTE.
    void set_attribute(const std::string& attribute, const std::string& value) override {
        if (attribute == "name") { name = value; return; }
        if (attribute == "service_id") { service_id = value; return; }
        if (attribute == "data_key") { data_key = value; return; }
        if (attribute == "condition") { condition = value; return; }

        // Normalize string-to-bool for pass_on_error.
        if (attribute == "pass_on_error") {
            std::string lower;
            lower.reserve(value.size());
            for (char c : value) {
                lower.push_back(static_cast<char>(
                    std::tolower(static_cast<unsigned char>(c))));
            }
            pass_on_error = (!lower.empty() && lower != "false" && lower != "0");
            return;
        }

        // Unknown attribute: delegate to base (raises error).
        Aggregate::set_attribute(attribute, value);
    }
};

// ** mapper: feature_event_yaml_object
/// YAML transfer object for FeatureEvent.
/// Uses MI: inherits data from FeatureEvent, serialization from TransferObject.
struct FeatureEventYamlObject : FeatureEvent, TransferObject {

    // * init: default
    FeatureEventYamlObject() = default;

    // * init: with fields
    FeatureEventYamlObject(std::string name,
                           std::string service_id,
                           std::unordered_map<std::string, std::string> parameters = {},
                           std::string data_key = "",
                           bool pass_on_error = false,
                           std::string condition = "")
        : FeatureEvent(std::move(name), std::move(service_id),
                       std::move(parameters), std::move(data_key),
                       pass_on_error, std::move(condition)) {}

    // * method: to_map
    /// Serialize scalar fields. Parameters map and type are excluded.
    /// Roles:
    ///   ""         — all scalar fields except type and parameters
    ///   "to_model" — same (type excluded per Python convention)
    ///   "to_data"  — same (type excluded per Python convention)
    FieldMap to_map(const std::string& role = "") const override {
        FieldMap m;
        if (!name.empty()) m["name"] = name;
        if (!service_id.empty()) m["service_id"] = service_id;
        if (!data_key.empty()) m["data_key"] = data_key;
        m["pass_on_error"] = pass_on_error ? "true" : "false";
        if (!condition.empty()) m["condition"] = condition;
        return m;
    }

    // * method: map
    /// Map to a FeatureEventAggregate.
    FeatureEventAggregate map() const {
        return FeatureEventAggregate{
            name, service_id, parameters,
            data_key, pass_on_error, condition};
    }

    // * method: from_model (static)
    /// Create from a FeatureEvent domain model or aggregate.
    static FeatureEventYamlObject from_model(const FeatureEvent& event) {
        return FeatureEventYamlObject{
            event.name, event.service_id, event.parameters,
            event.data_key, event.pass_on_error, event.condition};
    }
};

// ** mapper: feature_aggregate
/// Mutable Feature aggregate.
/// Uses MI: inherits data from Feature, mutation from Aggregate.
struct FeatureAggregate : Feature, Aggregate {

    // * init: default
    FeatureAggregate() = default;

    // * init: with fields
    FeatureAggregate(std::string id,
                     std::string name,
                     std::string group_id,
                     std::string feature_key,
                     std::vector<FeatureEvent> steps = {},
                     std::string description = "")
        : Feature(std::move(id), std::move(name), std::move(group_id),
                  std::move(feature_key), std::move(steps),
                  std::move(description)) {}

    // * method: set_attribute
    /// Dispatch to known Feature fields or raise INVALID_MODEL_ATTRIBUTE.
    void set_attribute(const std::string& attribute, const std::string& value) override {
        if (attribute == "name") { name = value; return; }
        if (attribute == "description") { description = value; return; }

        // Unknown attribute: delegate to base (raises error).
        Aggregate::set_attribute(attribute, value);
    }

    // * method: rename
    /// Update the feature name.
    void rename(const std::string& new_name) {
        name = new_name;
    }

    // * method: set_description
    /// Update the feature description.
    void set_description(const std::string& desc) {
        description = desc;
    }

    // * method: add_step
    /// Append a feature event step.
    void add_step(FeatureEvent step) {
        steps.push_back(std::move(step));
    }

    // * method: add_step (at position)
    /// Insert a feature event step at the given position.
    void add_step(FeatureEvent step, size_t position) {
        if (position >= steps.size()) {
            steps.push_back(std::move(step));
        } else {
            steps.insert(steps.begin() + static_cast<ptrdiff_t>(position),
                         std::move(step));
        }
    }
};

// ** mapper: feature_yaml_object
/// YAML transfer object for Feature.
/// Uses MI: inherits data from Feature, serialization from TransferObject.
struct FeatureYamlObject : Feature, TransferObject {

    // * init: default
    FeatureYamlObject() = default;

    // * init: with fields
    FeatureYamlObject(std::string id,
                      std::string name,
                      std::string group_id,
                      std::string feature_key,
                      std::vector<FeatureEvent> steps = {},
                      std::string description = "")
        : Feature(std::move(id), std::move(name), std::move(group_id),
                  std::move(feature_key), std::move(steps),
                  std::move(description)) {}

    // * method: to_map
    /// Serialize scalar fields. Steps list is excluded.
    /// Roles:
    ///   ""         — all scalar fields
    ///   "to_model" — all scalar fields (steps handled by map())
    ///   "to_data"  — exclude id, group_id, feature_key
    FieldMap to_map(const std::string& role = "") const override {
        FieldMap m;

        if (role == "to_data") {
            // Exclude derived keys for YAML storage.
            if (!name.empty()) m["name"] = name;
            if (!description.empty()) m["description"] = description;
        } else {
            // Default and to_model: all scalar fields.
            if (!id.empty()) m["id"] = id;
            if (!name.empty()) m["name"] = name;
            if (!group_id.empty()) m["group_id"] = group_id;
            if (!feature_key.empty()) m["feature_key"] = feature_key;
            if (!description.empty()) m["description"] = description;
        }
        return m;
    }

    // * method: map
    /// Map to a FeatureAggregate, carrying nested steps.
    FeatureAggregate map() const {
        return FeatureAggregate{
            id, name, group_id, feature_key,
            steps, description};
    }

    // * method: from_model (static)
    /// Create a FeatureYamlObject from a Feature domain model or aggregate.
    static FeatureYamlObject from_model(const Feature& feature) {
        return FeatureYamlObject{
            feature.id, feature.name, feature.group_id,
            feature.feature_key, feature.steps, feature.description};
    }
};

} // namespace tiferet

#endif // TIFERET_MAPPERS_FEATURE_H
