// *** tiferet/interfaces/feature.h
// Service interface for feature retrieval.
// Mirrors Python FeatureService (get, exists).

#ifndef TIFERET_INTERFACES_FEATURE_H
#define TIFERET_INTERFACES_FEATURE_H

#include <tiferet/interfaces/settings.h>
#include <tiferet/domain/feature.h>

#include <string>

namespace tiferet {

// *** interfaces

// ** interface: feature_service
/// Abstract service contract for retrieving Feature definitions.
/// Consumed by AppInterfaceContext and FeatureContext to load features
/// from persistent configuration (e.g., YAML).
class FeatureService : public Service {
public:

    // * method: destructor
    virtual ~FeatureService() = default;

    // * method: get
    /// Retrieve a feature by its composite identifier (group_id.feature_key).
    /// Returns nullptr if not found.
    ///
    /// :param id: The feature identifier.
    /// :return: Pointer to the feature, or nullptr.
    virtual const Feature* get(const std::string& id) const = 0;

    // * method: exists
    /// Check whether a feature exists by its identifier.
    ///
    /// :param id: The feature identifier.
    /// :return: True if the feature exists.
    virtual bool exists(const std::string& id) const = 0;
};

} // namespace tiferet

#endif // TIFERET_INTERFACES_FEATURE_H
