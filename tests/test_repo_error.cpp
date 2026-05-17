// *** tests/test_repo_error.cpp

#if TIFERET_HAS_YAML

#include <catch2/catch_test_macros.hpp>
#include <tiferet/repos/error.h>

#include <fstream>
#include <cstdio>
#include <string>

using namespace tiferet;

namespace {

// *** helpers

static std::string temp_path(const std::string& name) {
    return std::string(std::tmpnam(nullptr)) + "_" + name + ".yml";
}

static void write_file(const std::string& path, const std::string& content) {
    std::ofstream out(path);
    out << content;
}

} // anonymous namespace

// *** tests

// ** test: load_errors
TEST_CASE("ErrorYamlRepository loads error definitions", "[repos][error]") {
    auto path = temp_path("err_basic");
    write_file(path,
        "errors:\n"
        "  invalid_input:\n"
        "    name: Invalid Numeric Input\n"
        "    message:\n"
        "      - lang: en_US\n"
        "        text: 'Value must be a number'\n"
        "      - lang: es_ES\n"
        "        text: 'El valor debe ser un numero'\n"
    );

    ErrorYamlRepository repo(path);

    REQUIRE(repo.size() == 1);
    REQUIRE(repo.exists("invalid_input"));

    const Error* err = repo.get("invalid_input");
    REQUIRE(err != nullptr);
    REQUIRE(err->name == "Invalid Numeric Input");
    REQUIRE(err->error_code == "INVALID_INPUT");
    REQUIRE(err->message.size() == 2);
    REQUIRE(err->message[0].lang == "en_US");
    REQUIRE(err->message[0].text == "Value must be a number");
    REQUIRE(err->message[1].lang == "es_ES");

    std::remove(path.c_str());
}

// ** test: lookup_by_error_code
TEST_CASE("ErrorYamlRepository supports lookup by error_code", "[repos][error]") {
    auto path = temp_path("err_code");
    write_file(path,
        "errors:\n"
        "  division_by_zero:\n"
        "    name: Division By Zero\n"
        "    message:\n"
        "      - lang: en_US\n"
        "        text: 'Cannot divide by zero'\n"
    );

    ErrorYamlRepository repo(path);

    // Lookup by id.
    REQUIRE(repo.get("division_by_zero") != nullptr);

    // Lookup by derived error_code.
    REQUIRE(repo.exists("DIVISION_BY_ZERO"));
    const Error* err = repo.get("DIVISION_BY_ZERO");
    REQUIRE(err != nullptr);
    REQUIRE(err->id == "division_by_zero");
    REQUIRE(err->name == "Division By Zero");

    std::remove(path.c_str());
}

// ** test: format_message
TEST_CASE("ErrorYamlRepository errors support format_message", "[repos][error]") {
    auto path = temp_path("err_format");
    write_file(path,
        "errors:\n"
        "  invalid_input:\n"
        "    name: Invalid Input\n"
        "    message:\n"
        "      - lang: en_US\n"
        "        text: 'Invalid input provided'\n"
        "      - lang: es_ES\n"
        "        text: 'Entrada invalida'\n"
    );

    ErrorYamlRepository repo(path);

    const Error* err = repo.get("invalid_input");
    REQUIRE(err != nullptr);
    REQUIRE(err->format_message("en_US") == "Invalid input provided");
    REQUIRE(err->format_message("es_ES") == "Entrada invalida");
    REQUIRE(err->format_message("fr_FR") == "");

    std::remove(path.c_str());
}

// ** test: multiple_errors
TEST_CASE("ErrorYamlRepository loads multiple errors", "[repos][error]") {
    auto path = temp_path("err_multi");
    write_file(path,
        "errors:\n"
        "  invalid_input:\n"
        "    name: Invalid Input\n"
        "    message:\n"
        "      - lang: en_US\n"
        "        text: 'Bad input'\n"
        "  division_by_zero:\n"
        "    name: Division By Zero\n"
        "    message:\n"
        "      - lang: en_US\n"
        "        text: 'Cannot divide by zero'\n"
        "  feature_not_found:\n"
        "    name: Feature Not Found\n"
        "    message:\n"
        "      - lang: en_US\n"
        "        text: 'Feature not found'\n"
    );

    ErrorYamlRepository repo(path);

    REQUIRE(repo.size() == 3);
    REQUIRE(repo.exists("invalid_input"));
    REQUIRE(repo.exists("DIVISION_BY_ZERO"));
    REQUIRE(repo.exists("feature_not_found"));
    REQUIRE(!repo.exists("nonexistent"));
    REQUIRE(repo.get("nonexistent") == nullptr);

    std::remove(path.c_str());
}

#endif // TIFERET_HAS_YAML
