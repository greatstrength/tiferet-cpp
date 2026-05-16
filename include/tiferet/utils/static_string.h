// *** tiferet/utils/static_string.h
// A fixed-capacity, null-terminated string for static/stack allocation.
// Replaces std::string in TIFERET_ALLOC_STATIC mode.

#ifndef TIFERET_UTILS_STATIC_STRING_H
#define TIFERET_UTILS_STATIC_STRING_H

#include <cstddef>
#include <cstring>
#include <string_view>

namespace tiferet {

// *** classes

// ** class: static_string
/// Fixed-capacity string stored entirely on the stack.
/// N is the maximum number of characters (excluding null terminator).
template <std::size_t N>
class StaticString {
public:

    // * init: default constructor
    StaticString() noexcept : size_(0) {
        data_[0] = '\0';
    }

    // * init: from c-string
    StaticString(const char* str) noexcept : size_(0) {
        if (str) {
            while (size_ < N && str[size_] != '\0') {
                data_[size_] = str[size_];
                ++size_;
            }
        }
        data_[size_] = '\0';
    }

    // * init: from string_view
    StaticString(std::string_view sv) noexcept : size_(0) {
        const std::size_t len = sv.size() < N ? sv.size() : N;
        for (std::size_t i = 0; i < len; ++i)
            data_[i] = sv[i];
        size_ = len;
        data_[size_] = '\0';
    }

    // * method: c_str
    const char* c_str() const noexcept { return data_; }

    // * method: data
    const char* data() const noexcept { return data_; }

    // * method: size
    std::size_t size() const noexcept { return size_; }

    // * method: capacity
    static constexpr std::size_t capacity() noexcept { return N; }

    // * method: empty
    bool empty() const noexcept { return size_ == 0; }

    // * method: view
    std::string_view view() const noexcept { return {data_, size_}; }

    // * method: operator string_view
    operator std::string_view() const noexcept { return view(); }

    // * method: operator==
    bool operator==(const StaticString& other) const noexcept {
        return size_ == other.size_ && std::memcmp(data_, other.data_, size_) == 0;
    }

    bool operator==(const char* other) const noexcept {
        return view() == std::string_view(other);
    }

    // * method: operator!=
    bool operator!=(const StaticString& other) const noexcept {
        return !(*this == other);
    }

    // * method: operator[]
    char operator[](std::size_t index) const noexcept { return data_[index]; }

    // * method: clear
    void clear() noexcept {
        size_ = 0;
        data_[0] = '\0';
    }

    // * method: append
    bool append(std::string_view sv) noexcept {
        const std::size_t avail = N - size_;
        const std::size_t len = sv.size() < avail ? sv.size() : avail;
        for (std::size_t i = 0; i < len; ++i)
            data_[size_ + i] = sv[i];
        size_ += len;
        data_[size_] = '\0';
        return len == sv.size(); // true if fully appended
    }

private:

    // * attribute: data_
    char data_[N + 1];

    // * attribute: size_
    std::size_t size_;
};

} // namespace tiferet

#endif // TIFERET_UTILS_STATIC_STRING_H
