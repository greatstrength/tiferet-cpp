// *** tiferet/contexts/feature.h
// Feature execution pipeline for the Tiferet framework.
// FeatureContext loads features, resolves steps via the DI container,
// and executes them sequentially — mirroring Python FeatureContext.
//
// Domain events that participate in feature pipelines must implement
// the ExecutableEvent interface so FeatureContext can invoke them
// with a unified string-keyed data map.

#ifndef TIFERET_CONTEXTS_FEATURE_H
#define TIFERET_CONTEXTS_FEATURE_H

#include <tiferet/contexts/di.h>
#include <tiferet/domain/feature.h>
#include <tiferet/events/settings.h>
#include <tiferet/assets/constants.h>

#include <string>
#include <unordered_map>

namespace tiferet {

// *** types

// ** type: data_context
/// A string-keyed data bag passed between feature steps.
/// Mirrors Python RequestContext.data — a flat dict flowing through the pipeline.
using DataContext = std::unordered_map<std::string, std::string>;

// *** classes

// ** class: executable_event
/// Interface for domain events that can participate in the feature pipeline.
/// Provides a string-map execute() signature that FeatureContext calls uniformly.
///
/// Concrete events implement this alongside their typed DomainEvent interface:
///   class AddNumberEvent : public Service, public ExecutableEvent { ... };
class ExecutableEvent {
public:

    // * method: destructor
    virtual ~ExecutableEvent() = default;

    // * method: execute
    /// Execute the event with merged data and step parameters.
    /// Returns an optional result string. Empty string means no result.
    ///
    /// :param data: The current data context (read/write).
    /// :param params: Step-level parameters from FeatureEvent.parameters.
    /// :return: The result string, or empty if no output.
    virtual std::string execute(const DataContext& data,
                                const DataContext& params) = 0;

protected:

    // * init
    ExecutableEvent() = default;
};

// ** class: feature_context
/// Orchestrates the execution of a Feature's step pipeline.
/// Mirrors Python FeatureContext.execute_feature().
///
/// For each step in the feature:
///   1. Resolves the service from the DIContainer by service_id.
///   2. Casts to ExecutableEvent*.
///   3. Merges step parameters with the data context.
///   4. Calls execute(), storing the result under data_key if specified.
///   5. Honors pass_on_error to continue on failure.
///
/// Usage:
///   DIContainer container(configs, flags, registry);
///   FeatureContext ctx(container);
///   DataContext data = {{"a", "3"}, {"b", "4"}};
///   ctx.execute_feature(feature, data);
class FeatureContext {
public:

    // * init
    /// Construct a FeatureContext with a reference to the DI container.
    ///
    /// :param container: The DI container for resolving feature steps.
    explicit FeatureContext(const DIContainer& container)
        : container_(container) {}

    // Non-copyable, non-assignable (reference member).
    FeatureContext(const FeatureContext&) = delete;
    FeatureContext& operator=(const FeatureContext&) = delete;
    FeatureContext(FeatureContext&&) = default;
    FeatureContext& operator=(FeatureContext&&) = delete;

    // * method: execute_feature
    /// Execute a feature by iterating its steps, resolving each from the
    /// DI container, and invoking the event with merged data + parameters.
    ///
    /// :param feature: The feature definition to execute.
    /// :param data: The data context, modified in place as steps produce results.
    void execute_feature(const Feature& feature, DataContext& data) {

        // Iterate over the feature's configured steps.
        for (const auto& step : feature.steps) {

            // Resolve the service from the DI container.
            Service* svc = container_.get_service(step.service_id);

            // Raise an error if the service is not found.
            if (!svc) {
                DomainEvent::raise_error(
                    error_codes::FEATURE_COMMAND_LOADING_FAILED,
                    step.service_id.c_str()
                );
            }

            // Cast to ExecutableEvent for uniform invocation.
            auto* event = dynamic_cast<ExecutableEvent*>(svc);

            // Raise an error if the service does not implement ExecutableEvent.
            if (!event) {
                DomainEvent::raise_error(
                    error_codes::FEATURE_STEP_EXECUTION_FAILED,
                    step.service_id.c_str()
                );
            }

            // Execute the step, honoring pass_on_error.
            execute_step(*event, step, data);
        }
    }

private:

    // * method: execute_step
    /// Execute a single feature step with error handling.
    void execute_step(ExecutableEvent& event,
                      const FeatureEvent& step,
                      DataContext& data) {

#if TIFERET_EXCEPTIONS
        if (step.pass_on_error) {

            // Wrap execution in try/catch when pass_on_error is enabled.
            try {
                run_event(event, step, data);
            } catch (const TiferetException&) {
                // Swallow the error and continue to the next step.
            }
        } else {

            // Execute normally — exceptions propagate.
            run_event(event, step, data);
        }
#else
        // In no-exception mode, pass_on_error has no effect.
        run_event(event, step, data);
#endif
    }

    // * method: run_event
    /// Merge step parameters with data and invoke the event.
    void run_event(ExecutableEvent& event,
                   const FeatureEvent& step,
                   DataContext& data) {

        // Build the parameters: step.parameters override data values.
        DataContext merged_params = step.parameters;

        // Execute the event with the data context and merged parameters.
        std::string result = event.execute(data, merged_params);

        // Store the result under data_key if specified and non-empty.
        if (!step.data_key.empty() && !result.empty()) {
            data[step.data_key] = std::move(result);
        }
    }

    // * attribute: container_
    /// Reference to the DI container for resolving services.
    const DIContainer& container_;
};

} // namespace tiferet

#endif // TIFERET_CONTEXTS_FEATURE_H
