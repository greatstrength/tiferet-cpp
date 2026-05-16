// *** tests/test_arena.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/utils/arena_allocator.h>
#include <vector>

using namespace tiferet;

// *** tests

// ** test: arena_basic_allocation
TEST_CASE("Arena allocates and tracks usage", "[alloc][arena]") {
    char buffer[1024];
    Arena arena(buffer, sizeof(buffer));

    REQUIRE(arena.used() == 0);
    REQUIRE(arena.total() == 1024);

    void* ptr = arena.allocate(64);

    REQUIRE(ptr != nullptr);
    REQUIRE(arena.used() >= 64);
}

// ** test: arena_exhaustion
TEST_CASE("Arena returns nullptr when exhausted", "[alloc][arena]") {
    char buffer[32];
    Arena arena(buffer, sizeof(buffer));

    void* ptr = arena.allocate(64);

    REQUIRE(ptr == nullptr);
}

// ** test: arena_reset
TEST_CASE("Arena reset reclaims all memory", "[alloc][arena]") {
    char buffer[256];
    Arena arena(buffer, sizeof(buffer));

    arena.allocate(100);
    arena.allocate(100);
    REQUIRE(arena.used() >= 200);

    arena.reset();

    REQUIRE(arena.used() == 0);
    REQUIRE(arena.remaining() == 256);
}

// ** test: arena_allocator_with_vector
TEST_CASE("ArenaAllocator works with std::vector", "[alloc][arena_allocator]") {
    char buffer[4096];
    Arena arena(buffer, sizeof(buffer));
    ArenaAllocator<int> alloc(arena);

    std::vector<int, ArenaAllocator<int>> v(alloc);
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    REQUIRE(v.size() == 3);
    REQUIRE(v[0] == 1);
    REQUIRE(v[2] == 3);
    REQUIRE(arena.used() > 0);
}
