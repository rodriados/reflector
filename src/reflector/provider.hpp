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

/**
 * Provides the properties' description of a reflectible type.
 * @tparam T The type to be described and reflected over.
 * @tparam R The properties' types of the target reflection type.
 * @return The target type descriptor instance.
 */
template <typename T, typename ...R>
REFLECTOR_CONSTEXPR auto provide(R T::*...) noexcept
{
    return detail::descriptor_t<T, supertuple::tuple_t<R...>>();
}

REFLECTOR_END_NAMESPACE
