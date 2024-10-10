/**
 * Reflector: A simple struct reflection framework for C++17.
 * @file The reflectible-type descriptor type and function.
 * @author Rodrigo Siqueira <rodriados@gmail.com>
 * @copyright 2024-present Rodrigo Siqueira
 */
#pragma once

#include <utility>
#include <supertuple.hpp>

#include <reflector/environment.h>

REFLECTOR_BEGIN_NAMESPACE

namespace supertuple = ::SUPERTUPLE_NAMESPACE;

namespace detail
{
    /**
     * The descriptor of a type, which aggregates a reflectible type with a tuple
     * of its corresponding internal properties' types.
     * @tparam T The type to be reflected.
     * @since 1.0
     */
    template <typename T, typename>
    class descriptor_t;

    /**
     * The description entry of a specific reflectible type.
     * @tparam T The type to be reflected.
     * @tparam R The internal property types of the reflected type.
     * @since 1.0
     */
    template <typename T, typename ...R>
    class descriptor_t<T, supertuple::tuple_t<R...>>
    {
        public:
            typedef T target_t;

        static_assert(!std::is_union_v<T>, "union types cannot be reflected");

        private:
            /**
             * A generic aligned memory storage container.
             * @tparam S The number of bytes in storage.
             * @tparam A The byte alignment the storage should use.
             * @since 1.0
             */
            template <size_t S, size_t A>
            struct alignas(A) storage_t {
                alignas(A) uint8_t storage[S];
            };

        public:
            using reflection_tuple_t = supertuple::tuple_t<R...>;
            using reference_tuple_t = supertuple::tuple_t<R&...>;
            using storage_tuple_t = supertuple::tuple_t<storage_t<sizeof(R), alignof(R)>...>;

        static_assert(
            sizeof (target_t) == sizeof (reflection_tuple_t) &&
            alignof(target_t) == alignof(reflection_tuple_t)
          , "reflection tuple is not compatible with target type"
        );
    };
}

REFLECTOR_END_NAMESPACE
