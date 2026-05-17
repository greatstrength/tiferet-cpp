// *** tiferet/interfaces/error.h
// Service interface for error retrieval.
// Mirrors Python ErrorService (get, exists).

#ifndef TIFERET_INTERFACES_ERROR_H
#define TIFERET_INTERFACES_ERROR_H

#include <tiferet/interfaces/settings.h>
#include <tiferet/domain/error.h>

#include <string>

namespace tiferet {

// *** interfaces

// ** interface: error_service
/// Abstract service contract for retrieving Error definitions.
/// Consumed by ErrorContext to format error messages from persistent
/// configuration (e.g., YAML).
class ErrorService : public Service {
public:

    // * method: destructor
    virtual ~ErrorService() = default;

    // * method: get
    /// Retrieve an error by its identifier or error code.
    /// Accepts either the YAML key (e.g., "invalid_input") or the
    /// derived uppercase error code (e.g., "INVALID_INPUT").
    /// Returns nullptr if not found.
    ///
    /// :param id_or_code: The error identifier or error code.
    /// :return: Pointer to the error, or nullptr.
    virtual const Error* get(const std::string& id_or_code) const = 0;

    // * method: exists
    /// Check whether an error exists by its identifier or error code.
    ///
    /// :param id_or_code: The error identifier or error code.
    /// :return: True if the error exists.
    virtual bool exists(const std::string& id_or_code) const = 0;
};

} // namespace tiferet

#endif // TIFERET_INTERFACES_ERROR_H
