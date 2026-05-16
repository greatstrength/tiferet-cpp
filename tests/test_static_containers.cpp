// *** tests/test_static_containers.cpp

#include <catch2/catch_test_macros.hpp>
#include <tiferet/utils/static_string.h>
#include <tiferet/utils/static_vector.h>

using namespace tiferet;

// *** tests: static_string

// ** test: static_string_default
TEST_CASE("StaticString default is empty", "[alloc][static_string]") {
    StaticString<32> s;

    REQUIRE(s.empty());
    REQUIRE(s.size() == 0);
    REQUIRE(s.capacity() == 32);
}

// ** test: static_string_from_cstr
TEST_CASE("StaticString from c-string", "[alloc][static_string]") {
    StaticString<32> s("hello");

    REQUIRE(s.size() == 5);
    REQUIRE(s == "hello");
    REQUIRE(s.view() == "hello");
}

// ** test: static_string_truncation
TEST_CASE("StaticString truncates on overflow", "[alloc][static_string]") {
    StaticString<4> s("abcdefgh");

    REQUIRE(s.size() == 4);
    REQUIRE(s == "abcd");
}

// ** test: static_string_append
TEST_CASE("StaticString append", "[alloc][static_string]") {
    StaticString<10> s("abc");
    bool full = s.append("def");

    REQUIRE(full);
    REQUIRE(s == "abcdef");
}

// ** test: static_string_clear
TEST_CASE("StaticString clear", "[alloc][static_string]") {
    StaticString<16> s("data");
    s.clear();

    REQUIRE(s.empty());
}

// *** tests: static_vector

// ** test: static_vector_default
TEST_CASE("StaticVector default is empty", "[alloc][static_vector]") {
    StaticVector<int, 8> v;

    REQUIRE(v.empty());
    REQUIRE(v.size() == 0);
    REQUIRE(v.capacity() == 8);
}

// ** test: static_vector_push_back
TEST_CASE("StaticVector push_back and access", "[alloc][static_vector]") {
    StaticVector<int, 4> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);

    REQUIRE(v.size() == 3);
    REQUIRE(v[0] == 10);
    REQUIRE(v[1] == 20);
    REQUIRE(v[2] == 30);
}

// ** test: static_vector_full
TEST_CASE("StaticVector rejects push_back when full", "[alloc][static_vector]") {
    StaticVector<int, 2> v;
    REQUIRE(v.push_back(1));
    REQUIRE(v.push_back(2));
    REQUIRE_FALSE(v.push_back(3));
    REQUIRE(v.full());
}

// ** test: static_vector_pop_back
TEST_CASE("StaticVector pop_back", "[alloc][static_vector]") {
    StaticVector<int, 4> v;
    v.push_back(1);
    v.push_back(2);
    v.pop_back();

    REQUIRE(v.size() == 1);
    REQUIRE(v[0] == 1);
}

// ** test: static_vector_clear
TEST_CASE("StaticVector clear", "[alloc][static_vector]") {
    StaticVector<int, 4> v;
    v.push_back(1);
    v.push_back(2);
    v.clear();

    REQUIRE(v.empty());
}

// ** test: static_vector_range_for
TEST_CASE("StaticVector range-for iteration", "[alloc][static_vector]") {
    StaticVector<int, 4> v;
    v.push_back(10);
    v.push_back(20);

    int sum = 0;
    for (auto val : v)
        sum += val;

    REQUIRE(sum == 30);
}
