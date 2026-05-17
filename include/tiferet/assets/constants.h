// *** tiferet/assets/constants.h
// Error code string constants for the Tiferet framework.
// Mirrors error codes from Python (assets/constants.py) and .NET (Domain/ErrorCodes.cs).

#ifndef TIFERET_ASSETS_CONSTANTS_H
#define TIFERET_ASSETS_CONSTANTS_H

namespace tiferet {
namespace error_codes {

// *** error codes

// ** error: parameter validation
constexpr const char* COMMAND_PARAMETER_REQUIRED = "COMMAND_PARAMETER_REQUIRED";

// ** error: model validation
constexpr const char* INVALID_MODEL_ATTRIBUTE = "INVALID_MODEL_ATTRIBUTE";

// ** error: feature errors
constexpr const char* FEATURE_NOT_FOUND = "FEATURE_NOT_FOUND";
constexpr const char* FEATURE_ALREADY_EXISTS = "FEATURE_ALREADY_EXISTS";
constexpr const char* FEATURE_COMMAND_LOADING_FAILED = "FEATURE_COMMAND_LOADING_FAILED";

// ** error: error domain
constexpr const char* ERROR_NOT_FOUND = "ERROR_NOT_FOUND";
constexpr const char* ERROR_ALREADY_EXISTS = "ERROR_ALREADY_EXISTS";

// ** error: dependency injection
constexpr const char* DEPENDENCY_TYPE_NOT_FOUND = "DEPENDENCY_TYPE_NOT_FOUND";

// ** error: app
constexpr const char* APP_ERROR = "APP_ERROR";
constexpr const char* APP_INTERFACE_NOT_FOUND = "APP_INTERFACE_NOT_FOUND";

// ** error: request
constexpr const char* REQUEST_NOT_FOUND = "REQUEST_NOT_FOUND";
constexpr const char* PARAMETER_NOT_FOUND = "PARAMETER_NOT_FOUND";

// ** error: file / utility
constexpr const char* INVALID_FILE_MODE = "INVALID_FILE_MODE";
constexpr const char* INVALID_FILE_ENCODING = "INVALID_FILE_ENCODING";
constexpr const char* FILE_NOT_FOUND = "FILE_NOT_FOUND";
constexpr const char* INVALID_FILE = "INVALID_FILE";
constexpr const char* DIVISION_BY_ZERO = "DIVISION_BY_ZERO";
constexpr const char* INVALID_INPUT = "INVALID_INPUT";

// ** error: yaml
constexpr const char* YAML_FILE_NOT_FOUND = "YAML_FILE_NOT_FOUND";
constexpr const char* YAML_LOAD_ERROR = "YAML_LOAD_ERROR";
constexpr const char* YAML_SAVE_ERROR = "YAML_SAVE_ERROR";

// ** error: app interface
constexpr const char* INVALID_APP_INTERFACE_TYPE = "INVALID_APP_INTERFACE_TYPE";

// ** error: service registry / DI container
constexpr const char* SERVICE_NOT_REGISTERED = "SERVICE_NOT_REGISTERED";
constexpr const char* SERVICE_RESOLUTION_FAILED = "SERVICE_RESOLUTION_FAILED";
constexpr const char* FEATURE_STEP_EXECUTION_FAILED = "FEATURE_STEP_EXECUTION_FAILED";

} // namespace error_codes
} // namespace tiferet

#endif // TIFERET_ASSETS_CONSTANTS_H
