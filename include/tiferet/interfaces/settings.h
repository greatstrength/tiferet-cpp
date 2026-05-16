// *** tiferet/interfaces/settings.h
// Base interface for all Tiferet service contracts.
// Mirrors Python Service(ABC) and .NET IService.

#ifndef TIFERET_INTERFACES_SETTINGS_H
#define TIFERET_INTERFACES_SETTINGS_H

namespace tiferet {

// *** classes

// ** class: service
/// Abstract base for all service contracts.
/// Service interfaces define the vertical contracts consumed by
/// domain events via dependency injection.
class Service {
public:

    // * method: destructor
    virtual ~Service() = default;

protected:

    // * init
    Service() = default;

    // Non-copyable, movable.
    Service(const Service&) = delete;
    Service& operator=(const Service&) = delete;
    Service(Service&&) = default;
    Service& operator=(Service&&) = default;
};

} // namespace tiferet

#endif // TIFERET_INTERFACES_SETTINGS_H
