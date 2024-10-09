/**
 * Reflector: A simple struct reflection framework for C++17.
 * @file The type descriptor type and function.
 * @author Rodrigo Siqueira <rodriados@gmail.com>
 * @copyright 2024-present Rodrigo Siqueira
 */
#pragma once

#include <supertuple.hpp>

#include <reflector/environment.h>
#include <reflector/detail/loophole.hpp>

REFLECTOR_BEGIN_NAMESPACE

namespace supertuple = ::SUPERTUPLE_NAMESPACE;

namespace detail
{
    /**
     * A generic aligned memory storage container.
     * @tparam S The number of bytes in storage.
     * @tparam A The byte alignment the storage should use.
     * @since 1.0
     */
    template <size_t S, size_t A>
    struct alignas(A) storage_t
    {
        alignas(A) uint8_t storage[S];
    };

    /**
     * The descriptor of a type, which aggregates a reflectible type with a tuple
     * of its corresponding internal properties.
     * @tparam T The type to be reflected.
     * @since 1.0
     */
    template <typename T, typename>
    struct descriptor_t;

    /**
     * The description entry of a specific reflectible type.
     * @tparam T The type to be reflected.
     * @tparam R The internal property types of the reflected type.
     * @since 1.0
     */
    template <typename T, typename ...R>
    struct descriptor_t<T, supertuple::tuple_t<R...>>
    {
        using target_t = T;

        static_assert(!std::is_union<T>::value, "union types cannot be reflected");

        using reflection_tuple_t = supertuple::tuple_t<R...>;
        using reference_tuple_t = supertuple::tuple_t<R&...>;
        using storage_tuple_t = supertuple::tuple_t<storage_t<sizeof(R), alignof(R)>...>;

        static_assert(
            sizeof (target_t) == sizeof (reflection_tuple_t) &&
            alignof(target_t) == alignof(reflection_tuple_t)
          , "reflection tuple is not compatible with target type");
    };
}

#ifndef REFLECTOR_AVOID_LOOPHOLE
/**
 * Describes a reflectible type by using the automatic reflector loophole mechanism.
 * @tparam T The type to be described and reflected over.
 * @return The target type descriptor instance.
 */
template <typename T>
REFLECTOR_CONSTEXPR auto describe(T*) noexcept
{
    static_assert(std::is_trivial<T>::value, "reflected type must be trivial");
    return detail::descriptor_t<T, decltype(detail::loophole<T>())>();
}

#else
/**
 * Defines the function for describing a reflectible type. Fails if not specialized.
 * @tparam T The type to be described and reflected over.
 * @return The target type descriptor instance.
 */
template <typename T>
REFLECTOR_CONSTEXPR auto describe(T*) noexcept
{
    static_assert(
        !std::is_void<T>::value && std::is_void<T>::value
      , "no description was found for the required type, so it cannot be reflected");
}
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

/**
 * Defines a reflectible type. This function can be specialized to provide a reflection
 * description for specific types when it's not possible to use the automatic loophole.
 * @tparam T The type to be described and reflected over.
 * @return The target type descriptor instance.
 */
template <typename T>
REFLECTOR_CONSTEXPR auto describe() noexcept
{
    return describe<T>(nullptr);
}

REFLECTOR_END_NAMESPACE
