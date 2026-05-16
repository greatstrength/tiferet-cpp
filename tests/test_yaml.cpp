// *** tests/test_yaml.cpp

#if TIFERET_HAS_YAML

#include <catch2/catch_test_macros.hpp>
#include <tiferet/utils/yaml.h>
#include <tiferet/assets/constants.h>

#include <fstream>
#include <cstdio>
#include <string>

using namespace tiferet;

// *** helpers

// ** helper: temp_yaml_path
/// Generate a temporary file path for test YAML files.
static std::string temp_yaml_path(const std::string& name) {
    return std::string(std::tmpnam(nullptr)) + "_" + name + ".yml";
}

// ** helper: write_file
static void write_file(const std::string& path, const std::string& content) {
    std::ofstream out(path);
    out << content;
}

// *** tests

// ** test: yaml_load_valid_file
TEST_CASE("YamlLoader loads a valid YAML file", "[utils][yaml]") {
    auto path = temp_yaml_path("load_valid");
    write_file(path, "name: Test\nvalue: 42\n");

    YamlLoader loader(path);
    auto node = loader.load();

    REQUIRE(node["name"].as<std::string>() == "Test");
    REQUIRE(node["value"].as<int>() == 42);

    std::remove(path.c_str());
}

// ** test: yaml_load_static_method
TEST_CASE("YamlLoader::load static method works", "[utils][yaml]") {
    auto path = temp_yaml_path("load_static");
    write_file(path, "key: value\n");

    auto node = YamlLoader::load(path);

    REQUIRE(node["key"].as<std::string>() == "value");

    std::remove(path.c_str());
}

// ** test: yaml_load_nested
TEST_CASE("YamlLoader loads nested YAML structures", "[utils][yaml]") {
    auto path = temp_yaml_path("load_nested");
    write_file(path, "parent:\n  child: hello\n  list:\n    - one\n    - two\n");

    auto node = YamlLoader::load(path);

    REQUIRE(node["parent"]["child"].as<std::string>() == "hello");
    REQUIRE(node["parent"]["list"].size() == 2);
    REQUIRE(node["parent"]["list"][0].as<std::string>() == "one");

    std::remove(path.c_str());
}

#if TIFERET_EXCEPTIONS

// ** test: yaml_load_file_not_found
TEST_CASE("YamlLoader raises YAML_FILE_NOT_FOUND for missing file", "[utils][yaml]") {
    REQUIRE_THROWS_AS(
        YamlLoader::load("/nonexistent/path.yml"),
        TiferetException
    );

    try {
        YamlLoader::load("/nonexistent/path.yml");
    } catch (const TiferetException& e) {
        REQUIRE(std::string(e.error_code()) == error_codes::YAML_FILE_NOT_FOUND);
    }
}

#endif // TIFERET_EXCEPTIONS

// ** test: yaml_save_and_reload
TEST_CASE("YamlLoader save and reload round-trip", "[utils][yaml]") {
    auto path = temp_yaml_path("save_reload");

    // Build a node and save it.
    YAML::Node node;
    node["id"] = "test_error";
    node["name"] = "Test Error";
    node["code"] = 42;

    YamlLoader::save(path, node);

    // Reload and verify.
    auto loaded = YamlLoader::load(path);

    REQUIRE(loaded["id"].as<std::string>() == "test_error");
    REQUIRE(loaded["name"].as<std::string>() == "Test Error");
    REQUIRE(loaded["code"].as<int>() == 42);

    std::remove(path.c_str());
}

// ** test: yaml_save_nested
TEST_CASE("YamlLoader saves nested structures", "[utils][yaml]") {
    auto path = temp_yaml_path("save_nested");

    YAML::Node node;
    node["features"]["calc"]["add"]["name"] = "Add Number";
    node["features"]["calc"]["add"]["service"] = "add_event";

    YamlLoader::save(path, node);

    auto loaded = YamlLoader::load(path);
    REQUIRE(loaded["features"]["calc"]["add"]["name"].as<std::string>() == "Add Number");

    std::remove(path.c_str());
}

#endif // TIFERET_HAS_YAML
