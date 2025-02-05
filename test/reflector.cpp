/**
 * Reflector: A simple struct reflection framework for C++17.
 * @file Test cases for the loophole reflection mechanism.
 * @author Rodrigo Siqueira <rodriados@gmail.com>
 * @copyright 2025-present Rodrigo Siqueira
 */
#include <utility>

#include <reflector/api.h>
#include <supertuple/api.h>
#include <catch2/catch_test_macros.hpp>

#include "resources/shapes.hpp"

/**
 * Generates samples of points with both reflection mechanisms enabled.
 * @param x The sample x-coordinate value to use for both instances.
 * @param y The sample y-coordinate value to use for both instances.
 * @return The generated sample points.
 */
REFLECTOR_CONSTEXPR static auto generate_sample_2d_points(double x, double y) noexcept
{
    return supertuple::pair_t(
        shape::loophole::point_t<2> {x, y}
      , shape::provider::point_t<2> {x, y});
}

/**
 * A use-case for generic reflectible types via different mechanisms.
 * @since 1.0
 */
TEST_CASE("general use-case for reflectible shapes", "[loophole][provider]")
{
    auto [point1L, point1P] = generate_sample_2d_points(4., 5.);
    auto [point2L, point2P] = generate_sample_2d_points(6., 7.);

    auto lineL = shape::loophole::line_t<2> {point1L, point2L};
    auto lineP = shape::provider::line_t<2> {point1P, point2P};

    /**
     * Test whether a simple type can be automatically reflected using both reflection
     * mechanisms. The extracted variables must be references to the object.
     * @since 1.0
     */
    SECTION("can reflect over a simple structure") {
        auto [xL, yL] = reflector::reflection_t(point1L);
        auto [xP, yP] = reflector::reflection_t(point1P);

        STATIC_REQUIRE(std::is_same_v<decltype(xL), double&>);
        STATIC_REQUIRE(std::is_same_v<decltype(yL), double&>);
        STATIC_REQUIRE(std::is_same_v<decltype(xP), double&>);
        STATIC_REQUIRE(std::is_same_v<decltype(yP), double&>);

        REQUIRE(&xL == &point1L.coords[0]);
        REQUIRE(&yL == &point1L.coords[1]);
        REQUIRE(&xP == &point1P.coords[0]);
        REQUIRE(&yP == &point1P.coords[1]);
    }

    /**
     * Test whether a composed type, that is, with an object as a member can be
     * automatically reflected using both reflection mechanisms.
     * @since 1.0
     */
    SECTION("can reflect over a composed structure") {
        auto [p1L, p2L] = reflector::reflection_t(lineL);
        auto [p1P, p2P] = reflector::reflection_t(lineP);

        STATIC_REQUIRE(std::is_same_v<decltype(p1L), shape::loophole::point_t<2>&>);
        STATIC_REQUIRE(std::is_same_v<decltype(p2L), shape::loophole::point_t<2>&>);
        STATIC_REQUIRE(std::is_same_v<decltype(p1P), shape::provider::point_t<2>&>);
        STATIC_REQUIRE(std::is_same_v<decltype(p2P), shape::provider::point_t<2>&>);

        REQUIRE(&p1L == &lineL.a);
        REQUIRE(&p2L == &lineL.b);
        REQUIRE(&p1P == &lineP.a);
        REQUIRE(&p2P == &lineP.b);
    }
}
