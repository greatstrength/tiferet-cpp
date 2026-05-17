// *** tests/test_repo_app.cpp

#if TIFERET_HAS_YAML

#include <catch2/catch_test_macros.hpp>
#include <tiferet/repos/app.h>

#include <fstream>
#include <cstdio>
#include <string>

using namespace tiferet;

namespace {

static std::string temp_path(const std::string& name) {
    return std::string(std::tmpnam(nullptr)) + "_" + name + ".yml";
}

static void write_file(const std::string& path, const std::string& content) {
    std::ofstream out(path);
    out << content;
}

} // anonymous namespace

// *** tests

// ** test: load_simple_interface
TEST_CASE("AppYamlRepository loads a simple interface", "[repos][app]") {
    auto path = temp_path("app_simple");
    write_file(path,
        "interfaces:\n"
        "  basic_calc:\n"
        "    name: Basic Calculator\n"
        "    description: Perform basic calculator operations\n"
    );

    AppYamlRepository repo(path);

    REQUIRE(repo.size() == 1);
    REQUIRE(repo.exists("basic_calc"));

    const AppInterface* iface = repo.get("basic_calc");
    REQUIRE(iface != nullptr);
    REQUIRE(iface->name == "Basic Calculator");
    REQUIRE(iface->description == "Perform basic calculator operations");
    REQUIRE(iface->flags.size() == 1);
    REQUIRE(iface->flags[0] == "default");

    std::remove(path.c_str());
}

// ** test: load_interface_with_services
TEST_CASE("AppYamlRepository loads interface with service dependencies", "[repos][app]") {
    auto path = temp_path("app_services");
    write_file(path,
        "interfaces:\n"
        "  calc_cli:\n"
        "    name: Calculator CLI\n"
        "    module_path: tiferet.contexts.cli\n"
        "    class_name: CliContext\n"
        "    attrs:\n"
        "      cli_repo:\n"
        "        module_path: tiferet.proxies.yaml.cli\n"
        "        class_name: CliYamlProxy\n"
        "        params:\n"
        "          config_file: cli.yml\n"
    );

    AppYamlRepository repo(path);

    const AppInterface* iface = repo.get("calc_cli");
    REQUIRE(iface != nullptr);
    REQUIRE(iface->module_path == "tiferet.contexts.cli");
    REQUIRE(iface->class_name == "CliContext");
    REQUIRE(iface->services.size() == 1);
    REQUIRE(iface->services[0].service_id == "cli_repo");
    REQUIRE(iface->services[0].module_path == "tiferet.proxies.yaml.cli");
    REQUIRE(iface->services[0].parameters.at("config_file") == "cli.yml");

    std::remove(path.c_str());
}

// ** test: load_interface_with_flags
TEST_CASE("AppYamlRepository loads interface with custom flags", "[repos][app]") {
    auto path = temp_path("app_flags");
    write_file(path,
        "interfaces:\n"
        "  test_iface:\n"
        "    name: Test Interface\n"
        "    flags:\n"
        "      - production\n"
        "      - logging\n"
    );

    AppYamlRepository repo(path);

    const AppInterface* iface = repo.get("test_iface");
    REQUIRE(iface != nullptr);
    REQUIRE(iface->flags.size() == 2);
    REQUIRE(iface->flags[0] == "production");
    REQUIRE(iface->flags[1] == "logging");

    std::remove(path.c_str());
}

// ** test: multiple_interfaces
TEST_CASE("AppYamlRepository loads multiple interfaces", "[repos][app]") {
    auto path = temp_path("app_multi");
    write_file(path,
        "interfaces:\n"
        "  basic_calc:\n"
        "    name: Basic Calculator\n"
        "  calc_cli:\n"
        "    name: Calculator CLI\n"
    );

    AppYamlRepository repo(path);

    REQUIRE(repo.size() == 2);
    REQUIRE(repo.exists("basic_calc"));
    REQUIRE(repo.exists("calc_cli"));
    REQUIRE(!repo.exists("nonexistent"));
    REQUIRE(repo.get("nonexistent") == nullptr);

    std::remove(path.c_str());
}

#endif // TIFERET_HAS_YAML
