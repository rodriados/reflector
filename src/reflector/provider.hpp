/**
 * Reflector: A simple struct reflection framework for C++17.
 * @file The reflectible-type description provider.
 * @author Rodrigo Siqueira <rodriados@gmail.com>
 * @copyright 2024-present Rodrigo Siqueira
 */
#pragma once

#include <utility>
#include <supertuple.hpp>

#include <reflector/environment.h>
#include <reflector/detail/loophole.hpp>
#include <reflector/detail/descriptor.hpp>

REFLECTOR_BEGIN_NAMESPACE

namespace supertuple = ::SUPERTUPLE_NAMESPACE;

/**
 * The reflection provider for a specified type. A custom, and possibly generic,
 * provider can be specified by specializing it to the reflected type.
 * @tparam T The type to have its reflection provided.
 * @since 1.0
 */
template <typename T>
struct provider_t;

#ifndef REFLECTOR_AVOID_LOOPHOLE

/**
 * Provides type reflection by using the automatic reflection loophole mechanism.
 * @tparam T The type to have a reflection provided for.
 * @since 1.0
 */
template <typename T>
struct provider_t
{
    /**
     * Invokes the reflection loophole to describe the target type.
     * @return The type description provided by the automatic reflection mechanism.
     */
    REFLECTOR_CONSTEXPR static auto provide() noexcept
    {
        static_assert(std::is_trivial_v<T>, "reflected type must be trivial");
        return detail::descriptor_t<T, decltype(detail::loophole<T>())>();
    }
};

#endif

namespace detail
{
    /**
     * Flattens an unary tuple by its array elements.
     * @tparam T The unary tuple type to be flattened.
     * @return The flattened unary tuple type.
     */
    template <
        typename T
      , typename E = std::remove_all_extents_t<T>
      , size_t N   = sizeof(T) / sizeof(E)>
    REFLECTOR_CONSTEXPR auto flatten(supertuple::tuple_t<T>) noexcept
    -> typename supertuple::ntuple_t<E, N>::base_tuple_t;
}

/**
 * Provides the properties' description of a reflectible type.
 * @tparam T The type to be described and reflected over.
 * @tparam R The properties' types of the target reflection type.
 * @return The target type descriptor instance.
 */
template <typename T, typename ...R>
REFLECTOR_CONSTEXPR auto provide(R T::*...) noexcept
{
    using namespace supertuple;

    // When a type is reflected via the loophole mechanism, array fields present
    // in the type are flattened, effectivelly creating a different index for each
    // element of each array field. On the other hand, when the type's fields are
    // manually provided, these array fields become a single index in the reflection.
    // This behaviour difference is not useful. Therefore, we must flatten these
    // array fields to produce consistent behaviour between different mechanisms.

    constexpr auto concatenate = [](auto a, auto b) {
        return supertuple::concat(a, b);
    };

    using loophole_tuple_t = decltype(
        supertuple::foldl(
            tuple_t<decltype(detail::flatten(tuple_t<R>()))...>()
          , concatenate
        ));

    return detail::descriptor_t<T, std::decay_t<loophole_tuple_t>>();
}

REFLECTOR_END_NAMESPACE
