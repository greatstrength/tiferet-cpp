// *** tests/test_service.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/interfaces/settings.h>
#include <memory>

using namespace tiferet;

// *** fixtures

// ** fixture: mock_calculator_service
/// A concrete service interface for testing.
class ICalculatorService : public Service {
public:
    virtual int compute(int a, int b) = 0;
};

// ** fixture: adder_service
class AdderService : public ICalculatorService {
public:
    int compute(int a, int b) override { return a + b; }
};

// *** tests

// ** test: service_interface_polymorphism
TEST_CASE("Service interface supports polymorphism", "[interfaces]") {
    std::unique_ptr<ICalculatorService> svc = std::make_unique<AdderService>();

    REQUIRE(svc->compute(3, 4) == 7);
}

// ** test: service_is_movable
TEST_CASE("Service is movable", "[interfaces]") {
    auto svc = std::make_unique<AdderService>();
    std::unique_ptr<ICalculatorService> moved = std::move(svc);

    REQUIRE(moved->compute(10, 5) == 15);
}
