// *** tiferet/mappers/error.h
// Error mapper classes for the Tiferet framework.
// ErrorAggregate provides mutable Error operations via MI (Error + Aggregate).
// ErrorYamlObject provides serialization via MI (Error + TransferObject).

#ifndef TIFERET_MAPPERS_ERROR_H
#define TIFERET_MAPPERS_ERROR_H

#include <tiferet/domain/error.h>
#include <tiferet/mappers/settings.h>

#include <algorithm>
#include <string>
#include <vector>

namespace tiferet {

// *** mappers

// ** mapper: error_aggregate
/// Mutable Error aggregate. Provides mutation methods for error data.
/// Uses MI: inherits data fields from Error, mutation infrastructure from Aggregate.
struct ErrorAggregate : Error, Aggregate {

    // * init: default
    ErrorAggregate() = default;

    // * init: with fields
    ErrorAggregate(std::string id,
                   std::string name,
                   std::vector<ErrorMessage> message = {},
                   std::string description = "",
                   std::string error_code = "")
        : Error(std::move(id), std::move(name), std::move(message),
                std::move(description), std::move(error_code)) {}

    // * method: set_attribute
    /// Dispatch to known Error fields or raise INVALID_MODEL_ATTRIBUTE.
    void set_attribute(const std::string& attribute, const std::string& value) override {
        if (attribute == "name") { name = value; return; }
        if (attribute == "description") { description = value; return; }
        if (attribute == "error_code") { error_code = value; return; }

        // Unknown attribute: delegate to base (raises error).
        Aggregate::set_attribute(attribute, value);
    }

    // * method: rename
    /// Update the error name.
    void rename(const std::string& new_name) {
        name = new_name;
    }

    // * method: set_message
    /// Set (or update) the message for a given language.
    void set_message(const std::string& lang, const std::string& text) {

        // Update existing message in place if a matching language is found.
        for (auto& msg : message) {
            if (msg.lang == lang) {
                msg.text = text;
                return;
            }
        }

        // If not found, append a new message.
        message.emplace_back(lang, text);
    }

    // * method: remove_message
    /// Remove the message for a given language.
    void remove_message(const std::string& lang) {
        message.erase(
            std::remove_if(message.begin(), message.end(),
                [&lang](const ErrorMessage& m) { return m.lang == lang; }),
            message.end());
    }
};

// ** mapper: error_message_yaml_object
/// YAML transfer object for ErrorMessage.
/// Uses MI: inherits data fields from ErrorMessage, serialization from TransferObject.
struct ErrorMessageYamlObject : ErrorMessage, TransferObject {

    // * init: default
    ErrorMessageYamlObject() = default;

    // * init: with fields
    ErrorMessageYamlObject(std::string lang, std::string text)
        : ErrorMessage(std::move(lang), std::move(text)) {}

    // * method: to_map
    FieldMap to_map(const std::string& role = "") const override {
        FieldMap m;
        if (!lang.empty()) m["lang"] = lang;
        if (!text.empty()) m["text"] = text;
        return m;
    }

    // * method: map
    /// Map to an ErrorMessage domain object.
    ErrorMessage map() const {
        return ErrorMessage{lang, text};
    }

    // * method: from_model (static)
    /// Create from an ErrorMessage domain object.
    static ErrorMessageYamlObject from_model(const ErrorMessage& msg) {
        return ErrorMessageYamlObject{msg.lang, msg.text};
    }
};

// ** mapper: error_yaml_object
/// YAML transfer object for Error.
/// Uses MI: inherits data fields from Error, serialization from TransferObject.
struct ErrorYamlObject : Error, TransferObject {

    // * init: default
    ErrorYamlObject() = default;

    // * init: with fields
    ErrorYamlObject(std::string id,
                    std::string name,
                    std::vector<ErrorMessage> message = {},
                    std::string description = "",
                    std::string error_code = "")
        : Error(std::move(id), std::move(name), std::move(message),
                std::move(description), std::move(error_code)) {}

    // * method: to_map
    /// Serialize scalar fields. Nested message list is excluded.
    /// Roles:
    ///   ""         — all scalar fields
    ///   "to_model" — all scalar fields (message handled by map())
    ///   "to_data"  — exclude id (derived at load time)
    FieldMap to_map(const std::string& role = "") const override {
        FieldMap m;

        // Exclude id for to_data role.
        if (role != "to_data" && !id.empty()) m["id"] = id;
        if (!name.empty()) m["name"] = name;
        if (!description.empty()) m["description"] = description;
        if (!error_code.empty()) m["error_code"] = error_code;

        return m;
    }

    // * method: map
    /// Map to an ErrorAggregate, carrying nested messages.
    ErrorAggregate map() const {
        return ErrorAggregate{id, name, message, description, error_code};
    }

    // * method: from_model (static)
    /// Create an ErrorYamlObject from an Error domain model or aggregate.
    static ErrorYamlObject from_model(const Error& error) {
        return ErrorYamlObject{
            error.id, error.name, error.message,
            error.description, error.error_code};
    }
};

} // namespace tiferet

#endif // TIFERET_MAPPERS_ERROR_H
