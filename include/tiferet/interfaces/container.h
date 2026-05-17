// *** tiferet/interfaces/container.h
// Service interface for DI container configuration.
// Mirrors Python ContainerService (list).

#ifndef TIFERET_INTERFACES_CONTAINER_H
#define TIFERET_INTERFACES_CONTAINER_H

#include <tiferet/interfaces/settings.h>
#include <tiferet/domain/di.h>

#include <vector>

namespace tiferet {

// *** interfaces

// ** interface: container_service
/// Abstract service contract for retrieving DI service configurations.
/// Consumed by DIContainer to resolve all service configurations
/// from persistent configuration (e.g., YAML).
class ContainerService : public Service {
public:

    // * method: destructor
    virtual ~ContainerService() = default;

    // * method: list
    /// Retrieve all service configurations.
    /// Returns a const reference to the internal configuration list.
    ///
    /// :return: Const reference to the vector of service configurations.
    virtual const std::vector<ServiceConfiguration>& list() const = 0;
};

} // namespace tiferet

#endif // TIFERET_INTERFACES_CONTAINER_H
