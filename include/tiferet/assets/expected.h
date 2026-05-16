// *** tiferet/assets/expected.h
// A lightweight Expected<T> result type for C++17.
// Used as the error-propagation mechanism when TIFERET_EXCEPTIONS is OFF.
// Holds either a value of type T or a TiferetError.

#ifndef TIFERET_ASSETS_EXPECTED_H
#define TIFERET_ASSETS_EXPECTED_H

#include <new>
#include <type_traits>
#include <utility>

namespace tiferet {

// *** forward declarations
struct TiferetError;

// *** classes

// ** class: unexpected
/// Wrapper to explicitly construct an Expected in the error state.
template <typename E>
class Unexpected {
public:
    explicit Unexpected(const E& error) : error_(error) {}
    explicit Unexpected(E&& error) : error_(std::move(error)) {}

    const E& error() const& { return error_; }
    E&& error() && { return std::move(error_); }

private:
    E error_;
};

// ** class: expected
/// Holds either a value of type T or an error of type E.
/// Modeled after std::expected (C++23) for use with C++17.
template <typename T, typename E = TiferetError>
class Expected {
public:

    // * init: value constructors
    Expected(const T& value) : has_value_(true) {
        new (&storage_.value) T(value);
    }

    Expected(T&& value) : has_value_(true) {
        new (&storage_.value) T(std::move(value));
    }

    // * init: error constructor
    Expected(const Unexpected<E>& unexpected) : has_value_(false) {
        new (&storage_.error) E(unexpected.error());
    }

    Expected(Unexpected<E>&& unexpected) : has_value_(false) {
        new (&storage_.error) E(std::move(unexpected).error());
    }

    // * init: copy constructor
    Expected(const Expected& other) : has_value_(other.has_value_) {
        if (has_value_)
            new (&storage_.value) T(other.storage_.value);
        else
            new (&storage_.error) E(other.storage_.error);
    }

    // * init: move constructor
    Expected(Expected&& other) noexcept : has_value_(other.has_value_) {
        if (has_value_)
            new (&storage_.value) T(std::move(other.storage_.value));
        else
            new (&storage_.error) E(std::move(other.storage_.error));
    }

    // * method: destructor
    ~Expected() { destroy(); }

    // * method: copy assignment
    Expected& operator=(const Expected& other) {
        if (this != &other) {
            destroy();
            has_value_ = other.has_value_;
            if (has_value_)
                new (&storage_.value) T(other.storage_.value);
            else
                new (&storage_.error) E(other.storage_.error);
        }
        return *this;
    }

    // * method: move assignment
    Expected& operator=(Expected&& other) noexcept {
        if (this != &other) {
            destroy();
            has_value_ = other.has_value_;
            if (has_value_)
                new (&storage_.value) T(std::move(other.storage_.value));
            else
                new (&storage_.error) E(std::move(other.storage_.error));
        }
        return *this;
    }

    // * method: has_value
    bool has_value() const noexcept { return has_value_; }
    explicit operator bool() const noexcept { return has_value_; }

    // * method: value
    T& value() & { return storage_.value; }
    const T& value() const& { return storage_.value; }
    T&& value() && { return std::move(storage_.value); }

    // * method: error
    E& error() & { return storage_.error; }
    const E& error() const& { return storage_.error; }
    E&& error() && { return std::move(storage_.error); }

    // * method: value_or
    template <typename U>
    T value_or(U&& default_value) const& {
        return has_value_ ? storage_.value : static_cast<T>(std::forward<U>(default_value));
    }

private:

    // * attribute: storage
    union Storage {
        T value;
        E error;
        Storage() {}
        ~Storage() {}
    } storage_;

    // * attribute: has_value_
    bool has_value_;

    // * method: destroy
    void destroy() {
        if (has_value_)
            storage_.value.~T();
        else
            storage_.error.~E();
    }
};

// ** class: expected<void, E> specialization
/// Specialization for void return type (success/failure only).
template <typename E>
class Expected<void, E> {
public:

    // * init: success constructor
    Expected() : has_value_(true) {}

    // * init: error constructor
    Expected(const Unexpected<E>& unexpected) : has_value_(false), error_(unexpected.error()) {}
    Expected(Unexpected<E>&& unexpected) : has_value_(false), error_(std::move(unexpected).error()) {}

    // * method: has_value
    bool has_value() const noexcept { return has_value_; }
    explicit operator bool() const noexcept { return has_value_; }

    // * method: error
    E& error() & { return error_; }
    const E& error() const& { return error_; }

private:
    bool has_value_;
    E error_;
};

} // namespace tiferet

#endif // TIFERET_ASSETS_EXPECTED_H
