// *** tests/test_repo_feature.cpp

#if TIFERET_HAS_YAML

#include <catch2/catch_test_macros.hpp>
#include <tiferet/repos/feature.h>

#include <fstream>
#include <cstdio>
#include <string>

using namespace tiferet;

namespace {

// *** helpers

// ** helper: temp_path
static std::string temp_path(const std::string& name) {
    return std::string(std::tmpnam(nullptr)) + "_" + name + ".yml";
}

// ** helper: write_file
static void write_file(const std::string& path, const std::string& content) {
    std::ofstream out(path);
    out << content;
}

} // anonymous namespace

// *** tests

// ** test: load_single_feature
TEST_CASE("FeatureYamlRepository loads a single feature", "[repos][feature]") {
    auto path = temp_path("feat_single");
    write_file(path,
        "features:\n"
        "  calc:\n"
        "    add:\n"
        "      name: Add Number\n"
        "      description: Adds two numbers\n"
        "      commands:\n"
        "        - attribute_id: add_event\n"
        "          name: Add a and b\n"
    );

    FeatureYamlRepository repo(path);

    REQUIRE(repo.size() == 1);
    REQUIRE(repo.exists("calc.add"));

    const Feature* feat = repo.get("calc.add");
    REQUIRE(feat != nullptr);
    REQUIRE(feat->name == "Add Number");
    REQUIRE(feat->description == "Adds two numbers");
    REQUIRE(feat->group_id == "calc");
    REQUIRE(feat->feature_key == "add");
    REQUIRE(feat->steps.size() == 1);
    REQUIRE(feat->steps[0].service_id == "add_event");
    REQUIRE(feat->steps[0].name == "Add a and b");

    std::remove(path.c_str());
}

// ** test: load_feature_with_params
TEST_CASE("FeatureYamlRepository parses step parameters", "[repos][feature]") {
    auto path = temp_path("feat_params");
    write_file(path,
        "features:\n"
        "  calc:\n"
        "    sqrt:\n"
        "      name: Square Root\n"
        "      commands:\n"
        "        - attribute_id: exp_event\n"
        "          name: Calculate square root\n"
        "          params:\n"
        "            b: '0.5'\n"
        "          data_key: result\n"
    );

    FeatureYamlRepository repo(path);

    const Feature* feat = repo.get("calc.sqrt");
    REQUIRE(feat != nullptr);
    REQUIRE(feat->steps.size() == 1);
    REQUIRE(feat->steps[0].parameters.at("b") == "0.5");
    REQUIRE(feat->steps[0].data_key == "result");

    std::remove(path.c_str());
}

// ** test: load_multiple_groups
TEST_CASE("FeatureYamlRepository loads multiple groups and features", "[repos][feature]") {
    auto path = temp_path("feat_multi");
    write_file(path,
        "features:\n"
        "  calc:\n"
        "    add:\n"
        "      name: Add\n"
        "      commands:\n"
        "        - attribute_id: add_event\n"
        "          name: Add\n"
        "    multiply:\n"
        "      name: Multiply\n"
        "      commands:\n"
        "        - attribute_id: mul_event\n"
        "          name: Multiply\n"
        "  text:\n"
        "    append:\n"
        "      name: Append\n"
        "      commands:\n"
        "        - attribute_id: append_event\n"
        "          name: Append\n"
    );

    FeatureYamlRepository repo(path);

    REQUIRE(repo.size() == 3);
    REQUIRE(repo.exists("calc.add"));
    REQUIRE(repo.exists("calc.multiply"));
    REQUIRE(repo.exists("text.append"));
    REQUIRE(!repo.exists("calc.nonexistent"));

    std::remove(path.c_str());
}

// ** test: get_nonexistent_returns_nullptr
TEST_CASE("FeatureYamlRepository returns nullptr for missing feature", "[repos][feature]") {
    auto path = temp_path("feat_empty");
    write_file(path, "features:\n  calc:\n    add:\n      name: Add\n      commands: []\n");

    FeatureYamlRepository repo(path);

    REQUIRE(repo.get("calc.nonexistent") == nullptr);
    REQUIRE(!repo.exists("calc.nonexistent"));

    std::remove(path.c_str());
}

// ** test: multi_step_feature
TEST_CASE("FeatureYamlRepository loads multi-step features", "[repos][feature]") {
    auto path = temp_path("feat_multistep");
    write_file(path,
        "features:\n"
        "  calc:\n"
        "    pipeline:\n"
        "      name: Pipeline\n"
        "      commands:\n"
        "        - attribute_id: add_event\n"
        "          name: Step 1\n"
        "          data_key: sum\n"
        "        - attribute_id: mul_event\n"
        "          name: Step 2\n"
        "          data_key: product\n"
        "          pass_on_error: true\n"
    );

    FeatureYamlRepository repo(path);

    const Feature* feat = repo.get("calc.pipeline");
    REQUIRE(feat != nullptr);
    REQUIRE(feat->steps.size() == 2);
    REQUIRE(feat->steps[0].data_key == "sum");
    REQUIRE(feat->steps[0].pass_on_error == false);
    REQUIRE(feat->steps[1].data_key == "product");
    REQUIRE(feat->steps[1].pass_on_error == true);

    std::remove(path.c_str());
}

// ** test: service_id_alias
TEST_CASE("FeatureYamlRepository accepts service_id as alias for attribute_id", "[repos][feature]") {
    auto path = temp_path("feat_svcid");
    write_file(path,
        "features:\n"
        "  calc:\n"
        "    add:\n"
        "      name: Add\n"
        "      commands:\n"
        "        - service_id: add_event\n"
        "          name: Add\n"
    );

    FeatureYamlRepository repo(path);

    const Feature* feat = repo.get("calc.add");
    REQUIRE(feat != nullptr);
    REQUIRE(feat->steps[0].service_id == "add_event");

    std::remove(path.c_str());
}

#endif // TIFERET_HAS_YAML
