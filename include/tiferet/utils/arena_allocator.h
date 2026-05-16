// *** tiferet/utils/arena_allocator.h
// A linear (bump) arena allocator satisfying the C++ Allocator concept.
// Used in TIFERET_ALLOC_ARENA mode for per-request allocation.

#ifndef TIFERET_UTILS_ARENA_ALLOCATOR_H
#define TIFERET_UTILS_ARENA_ALLOCATOR_H

#include <cstddef>
#include <cstdint>
#include <new>

namespace tiferet {

// *** classes

// ** class: arena
/// A fixed-size memory arena that hands out blocks via bump allocation.
/// Owns its buffer (stack or heap) and supports bulk reset.
class Arena {
public:

    // * init: from external buffer
    Arena(void* buffer, std::size_t size) noexcept
        : buffer_(static_cast<char*>(buffer))
        , size_(size)
        , offset_(0)
    {}

    // * method: allocate
    /// Allocate `size` bytes with the given alignment.
    /// Returns nullptr if the arena is exhausted.
    void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) noexcept {
        // Align the current offset.
        std::size_t aligned = (offset_ + alignment - 1) & ~(alignment - 1);
        if (aligned + size > size_)
            return nullptr;

        void* ptr = buffer_ + aligned;
        offset_ = aligned + size;
        return ptr;
    }

    // * method: reset
    /// Bulk-free all allocations (O(1)).
    void reset() noexcept { offset_ = 0; }

    // * method: used
    std::size_t used() const noexcept { return offset_; }

    // * method: remaining
    std::size_t remaining() const noexcept { return size_ - offset_; }

    // * method: total
    std::size_t total() const noexcept { return size_; }

private:

    // * attribute: buffer_
    char* buffer_;

    // * attribute: size_
    std::size_t size_;

    // * attribute: offset_
    std::size_t offset_;
};

// ** class: arena_allocator
/// STL-compatible allocator backed by an Arena.
/// Satisfies the C++ Allocator concept so it can be plugged into
/// std::vector<T, ArenaAllocator<T>>, etc.
template <typename T>
class ArenaAllocator {
public:
    using value_type = T;

    // * init
    explicit ArenaAllocator(Arena& arena) noexcept : arena_(&arena) {}

    // * init: rebind copy constructor
    template <typename U>
    ArenaAllocator(const ArenaAllocator<U>& other) noexcept : arena_(other.arena_) {}

    // * method: allocate
    T* allocate(std::size_t n) {
        void* ptr = arena_->allocate(n * sizeof(T), alignof(T));
        if (!ptr) {
#if TIFERET_EXCEPTIONS
            throw std::bad_alloc();
#else
            return nullptr;
#endif
        }
        return static_cast<T*>(ptr);
    }

    // * method: deallocate (no-op for arena allocator)
    void deallocate(T*, std::size_t) noexcept {
        // Arena allocator: individual deallocation is a no-op.
        // Memory is reclaimed via Arena::reset().
    }

    // * method: operator==
    bool operator==(const ArenaAllocator& other) const noexcept {
        return arena_ == other.arena_;
    }

    bool operator!=(const ArenaAllocator& other) const noexcept {
        return arena_ != other.arena_;
    }

private:
    template <typename U> friend class ArenaAllocator;

    // * attribute: arena_
    Arena* arena_;
};

} // namespace tiferet

#endif // TIFERET_UTILS_ARENA_ALLOCATOR_H
