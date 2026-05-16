// *** tiferet/domain/error.h
// Error domain models for the Tiferet framework.
// ErrorMessage carries a localized message text.
// Error carries an error definition with code derivation and multilingual messages.
// Both use virtual DomainObject inheritance for MI with Aggregate/TransferObject.

#ifndef TIFERET_DOMAIN_ERROR_H
#define TIFERET_DOMAIN_ERROR_H

#include <tiferet/domain/settings.h>

#include <cctype>
#include <string>
#include <vector>

namespace tiferet {

// *** models

// ** model: error_message
/// A localized error message.
struct ErrorMessage : virtual DomainObject {

    // * attribute: lang
    /// Language code (e.g., "en_US", "es_ES").
    std::string lang;

    // * attribute: text
    /// The message text, optionally containing format placeholders.
    std::string text;

    // * init: default
    ErrorMessage() = default;

    // * init: with fields
    ErrorMessage(std::string lang, std::string text)
        : lang(std::move(lang)), text(std::move(text)) {}

    // * method: validate
    bool validate() const noexcept override {
        return !lang.empty() && !text.empty();
    }

    // * method: format
    /// Return the message text.
    /// Placeholder substitution deferred to a future alpha.
    const std::string& format() const { return text; }
};

// ** model: error
/// An error definition with identifier, name, derived error code,
/// and a list of localized messages.
struct Error : virtual DomainObject {

    // * attribute: id
    /// The unique identifier of the error (e.g., "invalid_input").
    std::string id;

    // * attribute: name
    /// The display name of the error.
    std::string name;

    // * attribute: description
    /// Optional description. Empty string means not set.
    std::string description;

    // * attribute: error_code
    /// Uppercase/underscore code derived from id when not explicitly set.
    std::string error_code;

    // * attribute: message
    /// Localized error messages.
    std::vector<ErrorMessage> message;

    // * init: default
    Error() = default;

    // * init: with fields
    Error(std::string id,
          std::string name,
          std::vector<ErrorMessage> message = {},
          std::string description = "",
          std::string error_code = "")
        : id(std::move(id))
        , name(std::move(name))
        , message(std::move(message))
        , description(std::move(description))
    {
        // Derive error_code from id when not explicitly provided.
        this->error_code = error_code.empty()
            ? derive_error_code(this->id)
            : std::move(error_code);
    }

    // * method: validate
    bool validate() const noexcept override {
        return !id.empty() && !name.empty();
    }

    // * method: format_message
    /// Find the message for the given language and return its text.
    /// Returns an empty string if no matching language is found.
    std::string format_message(const std::string& lang = "en_US") const {
        for (const auto& msg : message) {
            if (msg.lang == lang) return msg.format();
        }
        return "";
    }

    // * method: derive_error_code (static)
    /// Convert an id string to an uppercase, underscore-separated error code.
    /// Mirrors Python Error._derive_error_code.
    static std::string derive_error_code(const std::string& id) {
        std::string code;
        code.reserve(id.size());
        for (char c : id) {
            if (c == ' ') code.push_back('_');
            else code.push_back(static_cast<char>(
                std::toupper(static_cast<unsigned char>(c))));
        }
        return code;
    }
};

} // namespace tiferet

#endif // TIFERET_DOMAIN_ERROR_H
