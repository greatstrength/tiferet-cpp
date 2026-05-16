// *** tiferet/utils/static_vector.h
// A fixed-capacity vector stored entirely on the stack.
// Replaces std::vector in TIFERET_ALLOC_STATIC mode.

#ifndef TIFERET_UTILS_STATIC_VECTOR_H
#define TIFERET_UTILS_STATIC_VECTOR_H

#include <cstddef>
#include <new>
#include <utility>

namespace tiferet {

// *** classes

// ** class: static_vector
/// Fixed-capacity contiguous container with no heap allocation.
/// N is the maximum number of elements.
template <typename T, std::size_t N>
class StaticVector {
public:

    // * init: default constructor
    StaticVector() noexcept : size_(0) {}

    // * method: destructor
    ~StaticVector() { clear(); }

    // * init: copy constructor
    StaticVector(const StaticVector& other) : size_(0) {
        for (std::size_t i = 0; i < other.size_; ++i)
            push_back(other[i]);
    }

    // * init: move constructor
    StaticVector(StaticVector&& other) noexcept : size_(0) {
        for (std::size_t i = 0; i < other.size_; ++i)
            push_back(std::move(other[i]));
        other.clear();
    }

    // * method: copy assignment
    StaticVector& operator=(const StaticVector& other) {
        if (this != &other) {
            clear();
            for (std::size_t i = 0; i < other.size_; ++i)
                push_back(other[i]);
        }
        return *this;
    }

    // * method: move assignment
    StaticVector& operator=(StaticVector&& other) noexcept {
        if (this != &other) {
            clear();
            for (std::size_t i = 0; i < other.size_; ++i)
                push_back(std::move(other[i]));
            other.clear();
        }
        return *this;
    }

    // * method: push_back
    bool push_back(const T& value) noexcept {
        if (size_ >= N) return false;
        new (data() + size_) T(value);
        ++size_;
        return true;
    }

    bool push_back(T&& value) noexcept {
        if (size_ >= N) return false;
        new (data() + size_) T(std::move(value));
        ++size_;
        return true;
    }

    // * method: emplace_back
    template <typename... Args>
    bool emplace_back(Args&&... args) noexcept {
        if (size_ >= N) return false;
        new (data() + size_) T(std::forward<Args>(args)...);
        ++size_;
        return true;
    }

    // * method: pop_back
    void pop_back() noexcept {
        if (size_ > 0) {
            --size_;
            data()[size_].~T();
        }
    }

    // * method: clear
    void clear() noexcept {
        for (std::size_t i = 0; i < size_; ++i)
            data()[i].~T();
        size_ = 0;
    }

    // * method: operator[]
    T& operator[](std::size_t index) noexcept { return data()[index]; }
    const T& operator[](std::size_t index) const noexcept { return data()[index]; }

    // * method: size
    std::size_t size() const noexcept { return size_; }

    // * method: capacity
    static constexpr std::size_t capacity() noexcept { return N; }

    // * method: empty
    bool empty() const noexcept { return size_ == 0; }

    // * method: full
    bool full() const noexcept { return size_ == N; }

    // * method: data
    T* data() noexcept { return reinterpret_cast<T*>(storage_); }
    const T* data() const noexcept { return reinterpret_cast<const T*>(storage_); }

    // * method: begin/end (iterator support)
    T* begin() noexcept { return data(); }
    T* end() noexcept { return data() + size_; }
    const T* begin() const noexcept { return data(); }
    const T* end() const noexcept { return data() + size_; }

private:

    // * attribute: storage_ (aligned raw storage)
    alignas(T) char storage_[sizeof(T) * N];

    // * attribute: size_
    std::size_t size_;
};

} // namespace tiferet

#endif // TIFERET_UTILS_STATIC_VECTOR_H
