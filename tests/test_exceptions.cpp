// *** tests/test_exceptions.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/assets/constants.h>
#include <tiferet/assets/exceptions.h>

using namespace tiferet;

// *** tests: tiferet_error

// ** test: error_default
TEST_CASE("TiferetError default is empty", "[assets][error]") {
    TiferetError err;

    REQUIRE(err.error_code == nullptr);
    REQUIRE_FALSE(static_cast<bool>(err));
}

// ** test: error_with_code
TEST_CASE("TiferetError with code and message", "[assets][error]") {
    TiferetError err(error_codes::FEATURE_NOT_FOUND, "not found");

    REQUIRE(static_cast<bool>(err));
    REQUIRE(std::string(err.error_code) == "FEATURE_NOT_FOUND");
    REQUIRE(std::string(err.message) == "not found");
}

// ** test: error_equality
TEST_CASE("TiferetError equality by code", "[assets][error]") {
    TiferetError a(error_codes::FEATURE_NOT_FOUND);
    TiferetError b(error_codes::FEATURE_NOT_FOUND);
    TiferetError c(error_codes::APP_ERROR);

    REQUIRE(a == b);
    REQUIRE(a != c);
}

#if TIFERET_EXCEPTIONS

// *** tests: tiferet_exception

// ** test: exception_throw_catch
TEST_CASE("TiferetException is throwable and catchable", "[assets][exception]") {
    REQUIRE_THROWS_AS(
        throw TiferetException(error_codes::APP_ERROR, "test error"),
        TiferetException
    );
}

// ** test: exception_error_code
TEST_CASE("TiferetException carries error code", "[assets][exception]") {
    try {
        throw TiferetException(error_codes::DIVISION_BY_ZERO, "cannot divide by zero");
    } catch (const TiferetException& e) {
        REQUIRE(std::string(e.error_code()) == "DIVISION_BY_ZERO");
        REQUIRE(std::string(e.what()) == "cannot divide by zero");
    }
}

// ** test: exception_from_error
TEST_CASE("TiferetException from TiferetError", "[assets][exception]") {
    TiferetError err(error_codes::INVALID_INPUT, "bad input");

    try {
        throw TiferetException(err);
    } catch (const TiferetException& e) {
        REQUIRE(e.error() == err);
    }
}

// ** test: api_exception
TEST_CASE("TiferetApiException carries name", "[assets][api_exception]") {
    try {
        throw TiferetApiException(error_codes::APP_ERROR, "Application Error", "something broke");
    } catch (const TiferetApiException& e) {
        REQUIRE(std::string(e.name()) == "Application Error");
        REQUIRE(std::string(e.error_code()) == "APP_ERROR");
    }
}

#endif // TIFERET_EXCEPTIONS
