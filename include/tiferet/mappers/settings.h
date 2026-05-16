// *** tiferet/mappers/settings.h
// Base classes for the Tiferet mapper layer.
// Aggregate: mutable domain object mixin with validated set_attribute().
// TransferObject: serialization mixin with to_map() and role-based field control.
//
// Both use virtual inheritance from DomainObject so that concrete mappers
// can combine a domain model with a mixin via MI without duplicating the base:
//   struct ErrorAggregate : Error, Aggregate { ... };

#ifndef TIFERET_MAPPERS_SETTINGS_H
#define TIFERET_MAPPERS_SETTINGS_H

#include <tiferet/domain/settings.h>
#include <tiferet/events/settings.h>
#include <tiferet/assets/constants.h>

#include <string>
#include <unordered_map>

namespace tiferet {

// *** types

// ** type: field_map
/// Flat key-value representation of domain object scalar fields.
/// Used for serialization/deserialization of transfer objects.
/// Nested structures (lists, sub-objects) are excluded from the map
/// and handled directly by map() / from_model().
using FieldMap = std::unordered_map<std::string, std::string>;

// *** classes

// ** class: aggregate
/// Mutable mixin for domain objects.
/// Provides validated attribute mutation via set_attribute().
/// Concrete aggregates combine a domain model with Aggregate using MI:
///   struct ErrorAggregate : Error, Aggregate { ... };
///
/// Virtual inheritance from DomainObject resolves the diamond when composed
/// with domain model classes that also inherit DomainObject.
struct Aggregate : virtual DomainObject {

    // * method: destructor
    virtual ~Aggregate() = default;

    // * method: set_attribute
    /// Update a named attribute. Subclasses override to dispatch to their
    /// known fields and fall through to this base for unknown attributes,
    /// which raises INVALID_MODEL_ATTRIBUTE.
    virtual void set_attribute(const std::string& attribute, const std::string& value) {
        DomainEvent::raise_error(error_codes::INVALID_MODEL_ATTRIBUTE, attribute.c_str());
    }

protected:

    // * init
    Aggregate() = default;
    Aggregate(const Aggregate&) = default;
    Aggregate(Aggregate&&) = default;
    Aggregate& operator=(const Aggregate&) = default;
    Aggregate& operator=(Aggregate&&) = default;
};

// ** class: transfer_object
/// Serialization mixin for domain objects.
/// Provides to_map() for flat field serialization with role-based control.
/// Concrete transfer objects combine a domain model with TransferObject using MI:
///   struct ErrorYamlObject : Error, TransferObject { ... };
///
/// Virtual inheritance from DomainObject resolves the diamond when composed
/// with domain model classes that also inherit DomainObject.
struct TransferObject : virtual DomainObject {

    // * method: destructor
    virtual ~TransferObject() = default;

    // * method: to_map
    /// Serialize scalar fields to a flat key-value map.
    /// The role parameter controls which fields are included:
    ///   ""           all scalar fields (default)
    ///   "to_model"   fields needed to construct an Aggregate (excludes nested)
    ///   "to_data"    fields for persistent storage (may exclude derived keys)
    /// Empty-string values are excluded (mirrors Python exclude_none=True).
    virtual FieldMap to_map(const std::string& role = "") const = 0;

protected:

    // * init
    TransferObject() = default;
    TransferObject(const TransferObject&) = default;
    TransferObject(TransferObject&&) = default;
    TransferObject& operator=(const TransferObject&) = default;
    TransferObject& operator=(TransferObject&&) = default;
};

} // namespace tiferet

#endif // TIFERET_MAPPERS_SETTINGS_H
