// *** tiferet/tiferet.h
// Umbrella header for the Tiferet C++ framework.
// Include this single header to access the full public API surface.

#ifndef TIFERET_H
#define TIFERET_H

// *** assets — configuration, error types, result types
#include <tiferet/assets/alloc_config.h>
#include <tiferet/assets/constants.h>
#include <tiferet/assets/exceptions.h>
#include <tiferet/assets/expected.h>

// *** domain — domain object base and domain models
#include <tiferet/domain/settings.h>
#include <tiferet/domain/error.h>
#include <tiferet/domain/feature.h>
#include <tiferet/domain/app.h>
#include <tiferet/domain/di.h>

// *** events — domain event base
#include <tiferet/events/settings.h>

// *** interfaces — service contracts
#include <tiferet/interfaces/settings.h>
#include <tiferet/interfaces/feature.h>
#include <tiferet/interfaces/error.h>
#include <tiferet/interfaces/container.h>
#include <tiferet/interfaces/app.h>

// *** mappers — aggregates and transfer objects
#include <tiferet/mappers/settings.h>
#include <tiferet/mappers/error.h>
#include <tiferet/mappers/feature.h>
#include <tiferet/mappers/app.h>
#include <tiferet/mappers/di.h>

// *** contexts — runtime orchestration
#include <tiferet/contexts/registry.h>
#include <tiferet/contexts/di.h>
#include <tiferet/contexts/feature.h>
#include <tiferet/contexts/error.h>
#include <tiferet/contexts/app.h>

// *** utils — static containers (always available)
#include <tiferet/utils/static_string.h>
#include <tiferet/utils/static_vector.h>
#include <tiferet/utils/arena_allocator.h>

// *** utils — YAML (conditional)
#if TIFERET_HAS_YAML
#include <tiferet/utils/yaml.h>
#include <tiferet/repos/feature.h>
#include <tiferet/repos/error.h>
#include <tiferet/repos/di.h>
#include <tiferet/repos/app.h>
#endif

#endif // TIFERET_H
