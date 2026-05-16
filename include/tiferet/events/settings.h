// *** tiferet/events/settings.h
// Base class for domain events in the Tiferet framework.
// Provides verify(), raise_error(), typed execute(), and static handle().
// Supports both exception and Expected<T> error propagation modes.

#ifndef TIFERET_EVENTS_SETTINGS_H
#define TIFERET_EVENTS_SETTINGS_H

#include <tiferet/assets/constants.h>
#include <tiferet/assets/exceptions.h>
#include <tiferet/assets/expected.h>

#include <utility>

namespace tiferet {

// *** classes

// ** class: domain_event
/// Non-generic base for all domain events.
/// Provides verification and error-raising infrastructure.
class DomainEvent {
public:

    // * method: destructor
    virtual ~DomainEvent() = default;

    // * method: verify
    /// Verify an expression; raise a TiferetError if false.
    void verify(bool expression, const char* error_code, const char* message = nullptr) {
        if (!expression)
            raise_error(error_code, message);
    }

    // * method: raise_error (static)
    /// Raise a structured error.
    /// In exception mode: throws TiferetException.
    /// In no-exception mode: this is a fatal assertion (firmware halt).
    [[noreturn]] static void raise_error(const char* error_code, const char* message = nullptr) {
#if TIFERET_EXCEPTIONS
        throw TiferetException(error_code, message);
#else
        // In no-exception mode, halt. Firmware targets override this via linker.
        (void)error_code;
        (void)message;
        while (true) {} // Default: infinite loop (overridable via weak symbol)
#endif
    }

    // * method: handle (static)
    /// Instantiate an event with dependencies and execute it.
    /// This is the primary invocation pattern for testing and pipeline use.
    ///
    /// Usage:
    ///   auto result = DomainEvent::handle<MyEvent>(
    ///       MyEvent(service_ref),
    ///       MyParams{.id = "test"}
    ///   );
    template <typename TEvent, typename TParams>
    static auto handle(TEvent& event, TParams&& params)
        -> decltype(event.execute(std::forward<TParams>(params)))
    {
        return event.execute(std::forward<TParams>(params));
    }

    /// Overload: construct event in-place from an rvalue.
    template <typename TEvent, typename TParams>
    static auto handle(TEvent&& event, TParams&& params)
        -> decltype(event.execute(std::forward<TParams>(params)))
    {
        return event.execute(std::forward<TParams>(params));
    }

protected:

    // * init
    DomainEvent() = default;

    // Non-copyable, movable.
    DomainEvent(const DomainEvent&) = delete;
    DomainEvent& operator=(const DomainEvent&) = delete;
    DomainEvent(DomainEvent&&) = default;
    DomainEvent& operator=(DomainEvent&&) = default;
};

// ** class: typed_domain_event
/// Generic base for domain events with typed input and typed output.
/// Mirrors .NET DomainEvent<TParams, TResult> and Python execute(**kwargs).
template <typename TParams, typename TResult>
class TypedDomainEvent : public DomainEvent {
public:

    // * method: execute (typed)
    /// Execute the domain event with strongly-typed parameters.
    /// Subclasses must override this.
    virtual TResult execute(const TParams& params) = 0;

protected:
    TypedDomainEvent() = default;
};

} // namespace tiferet

#endif // TIFERET_EVENTS_SETTINGS_H
