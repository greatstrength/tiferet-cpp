// *** tiferet/contexts/error.h
// Error formatting context for the Tiferet framework.
// Provides error lookup and message formatting via ErrorService.
// Mirrors Python ErrorContext.

#ifndef TIFERET_CONTEXTS_ERROR_H
#define TIFERET_CONTEXTS_ERROR_H

#include <tiferet/interfaces/error.h>
#include <tiferet/domain/error.h>

#include <string>

namespace tiferet {

// *** contexts

// ** context: error_context
/// Provides error lookup and message formatting using an ErrorService.
/// Used by application code to format error messages when handling exceptions.
///
/// Usage:
///   ErrorContext error_ctx(error_service);
///   const Error* err = error_ctx.get_error("INVALID_INPUT");
///   std::string msg = error_ctx.format_error("INVALID_INPUT", "en_US");
class ErrorContext {
public:

    // * init
    /// Construct an ErrorContext with a reference to the error service.
    ///
    /// :param error_service: The error service for looking up error definitions.
    explicit ErrorContext(const ErrorService& error_service)
        : error_service_(error_service) {}

    // Non-copyable (reference member), movable.
    ErrorContext(const ErrorContext&) = delete;
    ErrorContext& operator=(const ErrorContext&) = delete;
    ErrorContext(ErrorContext&&) = default;
    ErrorContext& operator=(ErrorContext&&) = delete;

    // * method: get_error
    /// Retrieve an error definition by its identifier or error code.
    /// Returns nullptr if not found.
    ///
    /// :param id_or_code: The error identifier or error code.
    /// :return: Pointer to the error, or nullptr.
    const Error* get_error(const std::string& id_or_code) const {
        return error_service_.get(id_or_code);
    }

    // * method: format_error
    /// Look up an error and format its message for the given language.
    /// Returns an empty string if the error or language is not found.
    ///
    /// :param id_or_code: The error identifier or error code.
    /// :param lang: The language code (default: "en_US").
    /// :return: The formatted message, or empty string.
    std::string format_error(const std::string& id_or_code,
                             const std::string& lang = "en_US") const {

        // Look up the error definition.
        const Error* err = error_service_.get(id_or_code);
        if (!err) return "";

        // Format the message for the requested language.
        return err->format_message(lang);
    }

    // * method: exists
    /// Check whether an error definition exists.
    ///
    /// :param id_or_code: The error identifier or error code.
    /// :return: True if the error exists.
    bool exists(const std::string& id_or_code) const {
        return error_service_.exists(id_or_code);
    }

private:

    // * attribute: error_service_
    /// Reference to the error service for lookups.
    const ErrorService& error_service_;
};

} // namespace tiferet

#endif // TIFERET_CONTEXTS_ERROR_H
