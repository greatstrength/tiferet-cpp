// *** tiferet/domain/feature.h
// Feature domain models for the Tiferet framework.
// FeatureStep is the base workflow step.
// FeatureEvent extends FeatureStep with service binding and parameters.
// Feature carries the full feature definition with key derivation.

#ifndef TIFERET_DOMAIN_FEATURE_H
#define TIFERET_DOMAIN_FEATURE_H

#include <tiferet/domain/settings.h>

#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>

namespace tiferet {

// *** models

// ** model: feature_step
/// Base step in a feature workflow.
struct FeatureStep : virtual DomainObject {

    // * attribute: type
    /// The step type discriminator. Always "event" for FeatureEvent.
    std::string type = "event";

    // * attribute: name
    /// The display name of the step.
    std::string name;

    // * init: default
    FeatureStep() = default;

    // * init: with fields
    explicit FeatureStep(std::string name, std::string type = "event")
        : type(std::move(type)), name(std::move(name)) {}

    // * method: validate
    bool validate() const noexcept override {
        return !name.empty();
    }
};

// ** model: feature_event
/// A feature step that executes a domain event from the container.
struct FeatureEvent : FeatureStep {

    // * attribute: service_id
    /// The service configuration ID for the feature event.
    std::string service_id;

    // * attribute: parameters
    /// Custom parameters for the feature event.
    std::unordered_map<std::string, std::string> parameters;

    // * attribute: data_key
    /// Optional key to store the result under in the data context.
    std::string data_key;

    // * attribute: pass_on_error
    /// Whether to continue execution on error.
    bool pass_on_error = false;

    // * attribute: condition
    /// Optional boolean expression for conditional execution.
    std::string condition;

    // * init: default
    FeatureEvent() = default;

    // * init: with fields
    FeatureEvent(std::string name,
                 std::string service_id,
                 std::unordered_map<std::string, std::string> parameters = {},
                 std::string data_key = "",
                 bool pass_on_error = false,
                 std::string condition = "")
        : FeatureStep(std::move(name))
        , service_id(std::move(service_id))
        , parameters(std::move(parameters))
        , data_key(std::move(data_key))
        , pass_on_error(pass_on_error)
        , condition(std::move(condition)) {}

    // * method: validate
    bool validate() const noexcept override {
        return FeatureStep::validate() && !service_id.empty();
    }
};

// ** model: feature
/// A feature definition with identifier, steps, and metadata.
struct Feature : virtual DomainObject {

    // * attribute: id
    /// The composite identifier (group_id.feature_key).
    std::string id;

    // * attribute: name
    /// The display name of the feature.
    std::string name;

    // * attribute: description
    /// Optional description. Defaults to name if not set.
    std::string description;

    // * attribute: group_id
    /// The context group identifier.
    std::string group_id;

    // * attribute: feature_key
    /// The key within the group.
    std::string feature_key;

    // * attribute: steps
    /// The step workflow for the feature.
    std::vector<FeatureEvent> steps;

    // * init: default
    Feature() = default;

    // * init: with fields
    Feature(std::string id,
            std::string name,
            std::string group_id,
            std::string feature_key,
            std::vector<FeatureEvent> steps = {},
            std::string description = "")
        : id(std::move(id))
        , name(std::move(name))
        , group_id(std::move(group_id))
        , feature_key(std::move(feature_key))
        , steps(std::move(steps))
        , description(std::move(description)) {}

    // * method: validate
    bool validate() const noexcept override {
        return !id.empty() && !name.empty()
            && !group_id.empty() && !feature_key.empty();
    }

    // * method: get_step
    /// Get the step at the given position, or nullptr if out of range.
    const FeatureEvent* get_step(size_t position) const {
        if (position >= steps.size()) return nullptr;
        return &steps[position];
    }

    // * method: get_step (mutable)
    FeatureEvent* get_step(size_t position) {
        if (position >= steps.size()) return nullptr;
        return &steps[position];
    }

    // * method: derive_keys (static)
    /// Derive id, group_id, feature_key, and description from whichever
    /// inputs are provided. Mirrors Python Feature.derive_keys validator.
    static void derive_keys(std::string& id,
                            std::string& group_id,
                            std::string& feature_key,
                            const std::string& name,
                            std::string& description) {

        // Derive group_id and feature_key from dotted id.
        if (!id.empty() && id.find('.') != std::string::npos
            && (group_id.empty() || feature_key.empty())) {
            auto dot = id.find('.');
            if (group_id.empty()) group_id = id.substr(0, dot);
            if (feature_key.empty()) feature_key = id.substr(dot + 1);
        }

        // Derive feature_key from name (snake-case).
        if (!name.empty() && feature_key.empty()) {
            feature_key.reserve(name.size());
            for (char c : name) {
                if (c == ' ') feature_key.push_back('_');
                else feature_key.push_back(static_cast<char>(
                    std::tolower(static_cast<unsigned char>(c))));
            }
        }

        // Derive id from group_id and feature_key.
        if (id.empty() && !group_id.empty() && !feature_key.empty()) {
            id = group_id + "." + feature_key;
        }

        // Default description to name.
        if (!name.empty() && description.empty()) {
            description = name;
        }
    }
};

} // namespace tiferet

#endif // TIFERET_DOMAIN_FEATURE_H
