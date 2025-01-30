/**
 * Reflector: A simple struct reflection framework for C++17.
 * @file Test cases for the manual provider reflection mechanism.
 * @author Rodrigo Siqueira <rodriados@gmail.com>
 * @copyright 2025-present Rodrigo Siqueira
 */
#define REFLECTOR_AVOID_LOOPHOLE
#define REFLECTOR_SHAPE_TEST_DEFINE_PROVIDERS

#include <reflector.h>
#include <catch2/catch_test_macros.hpp>

#include "resources/shapes.hpp"

/**
 * A use-case for generic and nested types reflected using the provider mechanism.
 * @since 1.0
 */
TEST_CASE("general use-case for the provider mechanism", "[provider]")
{
    auto point    = shape::point_t {4., 5.};
    auto circle   = shape::circle_t {point, 3.};
    auto cilinder = shape::cilinder_t {circle, 6.};

    /**
     * Test whether a simple struct can be automatically reflected using the provider
     * mechanism. The extracted variables must be references to the object.
     * @since 1.0
     */
    SECTION("can reflect over a simple structure") {
        auto [x, y] = reflector::reflection_t(point);

        REQUIRE(x == 4.);
        REQUIRE(y == 5.);
    }

    /**
     * Test whether a composed struct, that is, with another struct as a member
     * can be automatically reflected with the provider mechanism.
     * @since 1.0
     */
    SECTION("can reflect over a composed structure") {
        auto [p, radius] = reflector::reflection_t(circle);

        REQUIRE(p.coords[0] == 4.);
        REQUIRE(p.coords[1] == 5.);
        REQUIRE(radius == 3.);
    }

    /**
     * Test whether references are kept through multiple reflections over the same
     * object with the provider mechanism.
     * @since 1.0
     */
    SECTION("references are preserved over multiple reflections") {
        auto [c, height] = reflector::reflection_t(cilinder);
        auto [cpoint, _] = reflector::reflection_t(c);

        cpoint = shape::point_t {8., 9.};

        REQUIRE(cilinder.surface.center.coords[0] == 8.);
        REQUIRE(cilinder.surface.center.coords[1] == 9.);
    }
}
