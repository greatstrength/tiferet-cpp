// *** tiferet/interfaces/app.h
// Service interface for app interface retrieval.
// Mirrors Python AppService (get, exists).

#ifndef TIFERET_INTERFACES_APP_H
#define TIFERET_INTERFACES_APP_H

#include <tiferet/interfaces/settings.h>
#include <tiferet/domain/app.h>

#include <string>

namespace tiferet {

// *** interfaces

// ** interface: app_service
/// Abstract service contract for retrieving AppInterface definitions.
/// Consumed by the application layer to load interface configurations
/// from persistent configuration (e.g., YAML).
class AppService : public Service {
public:

    // * method: destructor
    virtual ~AppService() = default;

    // * method: get
    /// Retrieve an app interface by its identifier.
    /// Returns nullptr if not found.
    ///
    /// :param id: The interface identifier.
    /// :return: Pointer to the app interface, or nullptr.
    virtual const AppInterface* get(const std::string& id) const = 0;

    // * method: exists
    /// Check whether an app interface exists by its identifier.
    ///
    /// :param id: The interface identifier.
    /// :return: True if the interface exists.
    virtual bool exists(const std::string& id) const = 0;
};

} // namespace tiferet

#endif // TIFERET_INTERFACES_APP_H
