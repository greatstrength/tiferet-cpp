// *** tiferet/domain/settings.h
// Base class for all Tiferet domain objects.
// Domain objects are lightweight value types — no virtual methods, no heap required.
// Mutation logic lives on Aggregate subclasses in tiferet/mappers/.

#ifndef TIFERET_DOMAIN_SETTINGS_H
#define TIFERET_DOMAIN_SETTINGS_H

namespace tiferet {

// *** classes

// ** class: domain_object
/// Abstract base for all domain models.
/// Domain objects are read-only value types. They carry structured data
/// and may provide formatting/lookup methods but never mutate themselves.
/// Aggregates (in the mappers layer) extend domain objects with mutation.
struct DomainObject {

    // * method: destructor
    virtual ~DomainObject() = default;

    // * method: validate
    /// Subclasses override to perform field-level validation.
    /// Returns true if all fields are valid.
    /// The default implementation always succeeds.
    virtual bool validate() const noexcept { return true; }

protected:

    // * init: default
    DomainObject() = default;

    // * init: copy/move
    DomainObject(const DomainObject&) = default;
    DomainObject(DomainObject&&) = default;
    DomainObject& operator=(const DomainObject&) = default;
    DomainObject& operator=(DomainObject&&) = default;
};

} // namespace tiferet

#endif // TIFERET_DOMAIN_SETTINGS_H
