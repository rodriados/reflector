/**
 * Reflector: A simple struct reflection framework for C++17.
 * @file Test cases for the loophole reflection mechanism.
 * @author Rodrigo Siqueira <rodriados@gmail.com>
 * @copyright 2024-present Rodrigo Siqueira
 */
#include <catch.hpp>
#include <reflector.h>
#include <supertuple.h>

template <typename T>
struct point_t {
    T x, y;
};

template <typename T>
struct circle_t {
    point_t<T> center;
    T radius;
};

template <typename T>
struct cilinder_t {
    circle_t<T> surface;
    double height;
};

template <typename T = int>
inline cilinder_t<T> generate_cilinder() {
    auto point  = point_t<T> {4, 5};
    auto circle = circle_t<T> {point, 3};
    return cilinder_t<T> {circle, 4.5};
}

/**
 * A use-case for generic and nested types reflected using the loophole mechanism.
 * @since 1.0
 */
TEST_CASE("general use-case for the loophole mechanism", "[loophole]")
{
    auto cilinder = generate_cilinder();
    auto ref = reflector::reflection_t(cilinder);

    auto [circle, height] = ref;
    auto [point, radius]  = reflector::reflection_t(circle);
    auto [x, y]           = reflector::reflection_t(point);

    REQUIRE(cilinder.surface.center.x == 4);
    REQUIRE(cilinder.surface.center.y == 5);
    REQUIRE(cilinder.surface.radius == 3);
    REQUIRE(cilinder.height == 4.5);

    x = 10;
    y = 20;
    radius = 100;

    supertuple::set<1>(ref, -9.7);

    REQUIRE(cilinder.surface.center.x == 10);
    REQUIRE(cilinder.surface.center.y == 20);
    REQUIRE(cilinder.surface.radius == 100);
    REQUIRE(cilinder.height == -9.7);
}
