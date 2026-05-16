// *** tiferet/assets/exceptions.h
// Structured error types for the Tiferet framework.
// TiferetError is a lightweight value type (always available).
// TiferetException is a std::exception subclass (only when TIFERET_EXCEPTIONS=1).

#ifndef TIFERET_ASSETS_EXCEPTIONS_H
#define TIFERET_ASSETS_EXCEPTIONS_H

#include <cstring>
#include <string_view>

#if TIFERET_EXCEPTIONS
#include <stdexcept>
#include <string>
#endif

namespace tiferet {

// *** classes

// ** class: tiferet_error
/// Lightweight, non-throwing error descriptor.
/// Carries an error code and optional message.
/// Used as the E type in Expected<T, TiferetError>.
struct TiferetError {

    // * attribute: error_code
    /// The structured error code (e.g., "FEATURE_NOT_FOUND").
    const char* error_code = nullptr;

    // * attribute: message
    /// Optional human-readable message. May be nullptr.
    const char* message = nullptr;

    // * init: default
    TiferetError() noexcept = default;

    // * init: with code
    explicit TiferetError(const char* code, const char* msg = nullptr) noexcept
        : error_code(code), message(msg) {}

    // * method: operator bool
    explicit operator bool() const noexcept { return error_code != nullptr; }

    // * method: operator==
    bool operator==(const TiferetError& other) const noexcept {
        if (error_code == other.error_code) return true;
        if (error_code == nullptr || other.error_code == nullptr) return false;
        return std::strcmp(error_code, other.error_code) == 0;
    }

    bool operator!=(const TiferetError& other) const noexcept {
        return !(*this == other);
    }
};

#if TIFERET_EXCEPTIONS

// ** class: tiferet_exception
/// Exception type thrown when TIFERET_EXCEPTIONS is enabled.
/// Carries a TiferetError and exposes it via what().
class TiferetException : public std::runtime_error {
public:

    // * init
    explicit TiferetException(const char* error_code, const char* message = nullptr)
        : std::runtime_error(message ? message : error_code)
        , error_(error_code, message)
    {}

    explicit TiferetException(const TiferetError& error)
        : std::runtime_error(error.message ? error.message : error.error_code)
        , error_(error)
    {}

    // * method: error
    const TiferetError& error() const noexcept { return error_; }

    // * method: error_code
    const char* error_code() const noexcept { return error_.error_code; }

private:

    // * attribute: error_
    TiferetError error_;
};

// ** class: tiferet_api_exception
/// API-level exception with name and formatted message, for interface-level error responses.
class TiferetApiException : public TiferetException {
public:

    // * init
    TiferetApiException(const char* error_code, const char* name, const char* message)
        : TiferetException(error_code, message)
        , name_(name)
    {}

    // * method: name
    const char* name() const noexcept { return name_; }

private:

    // * attribute: name_
    const char* name_;
};

#endif // TIFERET_EXCEPTIONS

} // namespace tiferet

#endif // TIFERET_ASSETS_EXCEPTIONS_H
