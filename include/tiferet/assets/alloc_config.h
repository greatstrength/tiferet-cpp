// *** tiferet/assets/alloc_config.h
// Allocation mode configuration.
// Exactly one of TIFERET_ALLOC_HEAP, TIFERET_ALLOC_ARENA, or TIFERET_ALLOC_STATIC
// is defined to 1 by the build system. This header validates and provides
// convenience macros.

#ifndef TIFERET_ALLOC_CONFIG_H
#define TIFERET_ALLOC_CONFIG_H

// ** validation: ensure exactly one mode is active
#if !defined(TIFERET_ALLOC_HEAP) && !defined(TIFERET_ALLOC_ARENA) && !defined(TIFERET_ALLOC_STATIC)
    #define TIFERET_ALLOC_HEAP 1
#endif

#if (defined(TIFERET_ALLOC_HEAP) && TIFERET_ALLOC_HEAP) + \
    (defined(TIFERET_ALLOC_ARENA) && TIFERET_ALLOC_ARENA) + \
    (defined(TIFERET_ALLOC_STATIC) && TIFERET_ALLOC_STATIC) != 1
    #error "Exactly one of TIFERET_ALLOC_HEAP, TIFERET_ALLOC_ARENA, or TIFERET_ALLOC_STATIC must be set to 1."
#endif

// ** convenience: boolean queries
#if defined(TIFERET_ALLOC_HEAP) && TIFERET_ALLOC_HEAP
    #define TIFERET_HAS_HEAP 1
#else
    #define TIFERET_HAS_HEAP 0
#endif

#if defined(TIFERET_ALLOC_ARENA) && TIFERET_ALLOC_ARENA
    #define TIFERET_HAS_ARENA 1
#else
    #define TIFERET_HAS_ARENA 0
#endif

#if defined(TIFERET_ALLOC_STATIC) && TIFERET_ALLOC_STATIC
    #define TIFERET_HAS_STATIC 1
#else
    #define TIFERET_HAS_STATIC 0
#endif

// ** convenience: heap-capable (heap or arena modes can use dynamic containers)
#define TIFERET_HAS_DYNAMIC (TIFERET_HAS_HEAP || TIFERET_HAS_ARENA)

#endif // TIFERET_ALLOC_CONFIG_H
