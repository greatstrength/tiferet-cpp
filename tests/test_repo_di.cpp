// *** tests/test_repo_di.cpp

#if TIFERET_HAS_YAML

#include <catch2/catch_test_macros.hpp>
#include <tiferet/repos/di.h>

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

// ** test: load_basic_configs
TEST_CASE("DIYamlRepository loads basic service configurations", "[repos][di]") {
    auto path = temp_path("di_basic");
    write_file(path,
        "attrs:\n"
        "  add_event:\n"
        "    module_path: calc.events\n"
        "    class_name: AddNumber\n"
        "  mul_event:\n"
        "    module_path: calc.events\n"
        "    class_name: MultiplyNumber\n"
    );

    DIYamlRepository repo(path);

    REQUIRE(repo.size() == 2);
    const auto& configs = repo.list();
    REQUIRE(configs.size() == 2);

    // Verify first config (order may vary, check both).
    bool found_add = false, found_mul = false;
    for (const auto& cfg : configs) {
        if (cfg.id == "add_event") {
            REQUIRE(cfg.module_path == "calc.events");
            REQUIRE(cfg.class_name == "AddNumber");
            found_add = true;
        }
        if (cfg.id == "mul_event") {
            REQUIRE(cfg.module_path == "calc.events");
            REQUIRE(cfg.class_name == "MultiplyNumber");
            found_mul = true;
        }
    }
    REQUIRE(found_add);
    REQUIRE(found_mul);

    std::remove(path.c_str());
}

// ** test: load_with_parameters
TEST_CASE("DIYamlRepository parses service parameters", "[repos][di]") {
    auto path = temp_path("di_params");
    write_file(path,
        "attrs:\n"
        "  db_service:\n"
        "    module_path: infra.db\n"
        "    class_name: SqliteClient\n"
        "    params:\n"
        "      path: /tmp/test.db\n"
        "      mode: rw\n"
    );

    DIYamlRepository repo(path);

    const auto& configs = repo.list();
    REQUIRE(configs.size() == 1);
    REQUIRE(configs[0].id == "db_service");
    REQUIRE(configs[0].parameters.at("path") == "/tmp/test.db");
    REQUIRE(configs[0].parameters.at("mode") == "rw");

    std::remove(path.c_str());
}

// ** test: load_with_dependencies
TEST_CASE("DIYamlRepository parses flagged dependencies", "[repos][di]") {
    auto path = temp_path("di_deps");
    write_file(path,
        "attrs:\n"
        "  my_service:\n"
        "    module_path: default.module\n"
        "    class_name: DefaultClass\n"
        "    dependencies:\n"
        "      - module_path: prod.module\n"
        "        class_name: ProdClass\n"
        "        flag: production\n"
        "      - module_path: test.module\n"
        "        class_name: TestClass\n"
        "        flag: testing\n"
    );

    DIYamlRepository repo(path);

    const auto& configs = repo.list();
    REQUIRE(configs.size() == 1);
    REQUIRE(configs[0].dependencies.size() == 2);

    const auto* prod = configs[0].get_dependency("production");
    REQUIRE(prod != nullptr);
    REQUIRE(prod->module_path == "prod.module");
    REQUIRE(prod->class_name == "ProdClass");

    const auto* test = configs[0].get_dependency("testing");
    REQUIRE(test != nullptr);
    REQUIRE(test->module_path == "test.module");

    std::remove(path.c_str());
}

// ** test: empty_attrs
TEST_CASE("DIYamlRepository handles empty attrs section", "[repos][di]") {
    auto path = temp_path("di_empty");
    write_file(path, "attrs:\n");

    DIYamlRepository repo(path);

    REQUIRE(repo.size() == 0);
    REQUIRE(repo.list().empty());

    std::remove(path.c_str());
}

#endif // TIFERET_HAS_YAML
