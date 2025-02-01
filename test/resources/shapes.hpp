/**
 * Reflector: A simple struct reflection framework for C++17.
 * @file Simple shape-like structures for testing.
 * @author Rodrigo Siqueira <rodriados@gmail.com>
 * @copyright 2025-present Rodrigo Siqueira
 */
#pragma once

#include <reflector.h>

namespace shape::loophole
{
    /**
     * A simple n-dimensional point represented by a n-dimensional coordinate.
     * @tparam N The number of spacial dimensions.
     * @since 1.0
     */
    template <size_t N = 2>
    struct point_t { double coords[N]; };

    /**
     * A simple n-dimensional line segment represented by two points.
     * @tparam N The number of spacial dimensions.
     * @since 1.0
     */
    template <size_t N = 2>
    struct line_t { point_t<N> a, b; };
}

namespace shape::provider
{
    /**
     * A simple n-dimensional point represented by a n-dimensional coordinate.
     * @tparam N The number of spacial dimensions.
     * @since 1.0
     */
    template <size_t N = 2>
    struct point_t { double coords[N]; };

    /**
     * A simple n-dimensional line segment represented by two points.
     * @tparam N The number of spacial dimensions.
     * @since 1.0
     */
    template <size_t N = 2>
    struct line_t { point_t<N> a, b; };
}

/**
 * Specializes a reflector provider for a generic n-dimensional point, thus enabling
 * it for reflection via the explicit provider mechanism.
 * @tparam N The number of spacial dimensions.
 * @since 1.0
 */
template <size_t N>
struct reflector::provider_t<shape::provider::point_t<N>> {
    typedef shape::provider::point_t<N> point_t;
    REFLECTOR_CONSTEXPR static auto provide() noexcept {
        return reflector::provide(&point_t::coords);
    }
};

/**
 * Specializes a reflector provider for a generic n-dimensional line, thus enabling
 * it for reflection via the explicit provider mechanism.
 * @tparam N The number of spacial dimensions.
 * @since 1.0
 */
template <size_t N>
struct reflector::provider_t<shape::provider::line_t<N>> {
    typedef shape::provider::line_t<N> line_t;
    REFLECTOR_CONSTEXPR static auto provide() noexcept {
        return reflector::provide(&line_t::a, &line_t::b);
    }
};
