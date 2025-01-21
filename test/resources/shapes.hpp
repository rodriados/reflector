/**
 * Reflector: A simple struct reflection framework for C++17.
 * @file Simple shape-like structures for testing.
 * @author Rodrigo Siqueira <rodriados@gmail.com>
 * @copyright 2025-present Rodrigo Siqueira
 */
#pragma once

#ifdef REFLECTOR_SHAPE_TEST_DEFINE_PROVIDERS
  #include <reflector.h>
#endif

namespace shape
{
    /**
     * A simple two-dimensional point representation.
     * The point is represented as a coordinate pair of numbers.
     * @since 1.0
     */
    struct point_t {
        double coords[2];
    };

    /**
     * A simple two-dimensional circle representation.
     * The circle is represented by a center point and its radius.
     * @since 1.0
     */
    struct circle_t {
        point_t center;
        double radius;
    };

    /**
     * A simple three-dimensional non-rotated cilinder representation.
     * The cilinder is represented as a base circle and its height.
     * @since 1.0
     */
    struct cilinder_t {
        circle_t surface;
        double height;
    };
}

#ifdef REFLECTOR_SHAPE_TEST_DEFINE_PROVIDERS
/**
 * Specializes a reflector provider for a point, thus allowing it to be reflected
 * without the using the loophole mechanism.
 * @since 1.0
 */
template <>
struct reflector::provider_t<shape::point_t> {
    inline constexpr static auto provide() noexcept {
        return reflector::provide(&shape::point_t::coords);
    }
};

/**
 * Specializes a reflector provider for a circle, thus allowing it to be reflected
 * without the using the loophole mechanism.
 * @since 1.0
 */
template <>
struct reflector::provider_t<shape::circle_t> {
    inline constexpr static auto provide() noexcept {
        return reflector::provide(
            &shape::circle_t::center
          , &shape::circle_t::radius);
    }
};

/**
 * Specializes a reflector provider for a cilinder, thus allowing it to be reflected
 * without the using the loophole mechanism.
 * @since 1.0
 */
template <>
struct reflector::provider_t<shape::cilinder_t> {
    inline constexpr static auto provide() noexcept {
        return reflector::provide(
            &shape::cilinder_t::surface
          , &shape::cilinder_t::height);
    }
};
#endif
