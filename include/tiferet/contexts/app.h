// *** tiferet/contexts/app.h
// App interface context for the Tiferet framework.
// Ties together FeatureContext, ErrorContext, and feature lookup
// to provide a top-level run(feature_id, data) entry point.
// Mirrors Python AppInterfaceContext.

#ifndef TIFERET_CONTEXTS_APP_H
#define TIFERET_CONTEXTS_APP_H

#include <tiferet/contexts/feature.h>
#include <tiferet/contexts/error.h>
#include <tiferet/interfaces/feature.h>
#include <tiferet/assets/constants.h>

#include <string>

namespace tiferet {

// *** contexts

// ** context: app_interface_context
/// Top-level orchestration context for executing features.
/// Combines FeatureService (feature lookup), FeatureContext (pipeline execution),
/// and ErrorContext (error formatting) into a single run() entry point.
///
/// Usage:
///   AppInterfaceContext app_ctx(feature_service, error_service, container);
///   DataContext data = {{"a", "3"}, {"b", "4"}};
///   app_ctx.run("calc.add", data);
///   // data["result"] == "7"
class AppInterfaceContext {
public:

    // * init
    /// Construct an AppInterfaceContext with all required services.
    ///
    /// :param feature_service: The feature service for looking up features.
    /// :param error_service: The error service for formatting error messages.
    /// :param container: The DI container for resolving feature step services.
    AppInterfaceContext(const FeatureService& feature_service,
                       const ErrorService& error_service,
                       const DIContainer& container)
        : feature_service_(feature_service)
        , error_context_(error_service)
        , feature_ctx_(container) {}

    // Non-copyable (reference members), non-movable (FeatureContext has ref).
    AppInterfaceContext(const AppInterfaceContext&) = delete;
    AppInterfaceContext& operator=(const AppInterfaceContext&) = delete;
    AppInterfaceContext(AppInterfaceContext&&) = default;
    AppInterfaceContext& operator=(AppInterfaceContext&&) = delete;

    // * method: run
    /// Execute a feature by its identifier.
    /// Looks up the feature from the FeatureService and executes its
    /// step pipeline via FeatureContext. Results are stored in the data context.
    ///
    /// Raises FEATURE_NOT_FOUND if the feature does not exist.
    ///
    /// :param feature_id: The composite feature identifier (e.g., "calc.add").
    /// :param data: The data context, modified in place as steps produce results.
    void run(const std::string& feature_id, DataContext& data) {

        // Look up the feature.
        const Feature* feature = feature_service_.get(feature_id);

        // Raise an error if the feature is not found.
        if (!feature) {
            DomainEvent::raise_error(
                error_codes::FEATURE_NOT_FOUND,
                feature_id.c_str());
        }

        // Execute the feature pipeline.
        feature_ctx_.execute_feature(*feature, data);
    }

    // * method: error_context
    /// Access the error context for error formatting.
    ///
    /// :return: Const reference to the error context.
    const ErrorContext& error_context() const { return error_context_; }

private:

    // * attribute: feature_service_
    /// Reference to the feature service for lookups.
    const FeatureService& feature_service_;

    // * attribute: error_context_
    /// Error context for formatting error messages.
    ErrorContext error_context_;

    // * attribute: feature_ctx_
    /// Feature context for executing feature pipelines.
    FeatureContext feature_ctx_;
};

} // namespace tiferet

#endif // TIFERET_CONTEXTS_APP_H
