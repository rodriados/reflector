/**
 * Reflector: A simple struct reflection framework for C++17.
 * @file The great type loophole exploit implementation.
 * @author Rodrigo Siqueira <rodriados@gmail.com>
 * @copyright 2024-present Rodrigo Siqueira
 */
#pragma once

/*
 * The Great Type Loophole (C++14)
 * Initial implementation by Alexandr Poltavsky, http://alexpolt.github.io
 * With participation of Antony Polukhin, http://github.com/apolukhin
 *
 * The Great Type Loophole is a technique that allows to exchange type information
 * with template instantiations. Basically you can assign and read type information
 * during compile time. Here it is used to detect data members of a data type. It
 * was first described in this blog post http://alexpolt.github.io/type-loophole.html .
 *
 * This technique exploits the http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#2118
 * CWG 2118. Stateful metaprogramming via friend injection
 * Note: CWG agreed that such techniques should be ill-formed, although the mechanism
 * for prohibiting them is as yet undetermined.
 */
#include <cstddef>
#include <cstdint>
#include <utility>

#include <supertuple.hpp>
#include <reflector/environment.h>

#ifndef REFLECTOR_AVOID_LOOPHOLE
  #if REFLECTOR_CPP_DIALECT < 2014
    #define REFLECTOR_AVOID_LOOPHOLE
  #endif
#endif

#ifndef REFLECTOR_AVOID_LOOPHOLE

/*
 * As we are exploiting some "obscure" behaviors of the language, and using some
 * tricks that might upset compilers, we need to disable some warnings in order to
 * force the compilation to take place without any warnings.
 */
REFLECTOR_DISABLE_NVCC_WARNING_BEGIN(1301)
REFLECTOR_DISABLE_GCC_WARNING_BEGIN("-Wnon-template-friend")

REFLECTOR_BEGIN_NAMESPACE

namespace supertuple = ::SUPERTUPLE_NAMESPACE;

namespace detail
{
    /**
     * Tags a member property type to an index for overload resolution.
     * @tparam T The target type for reflection processing.
     * @tparam N The index of extracted property member type.
     * @since 1.0
     */
    template <typename T, size_t N>
    struct tag_t
    {
        REFLECTOR_CONSTEXPR friend auto latch(tag_t<T, N>) noexcept;
    };

    /**
     * Injects a friend function to couple a property type to its index.
     * @tparam T The target type for reflection processing.
     * @tparam P The extracted property type.
     * @param N The index of extracted property member type.
     * @since 1.0
     */
    template <typename T, typename P, size_t N>
    struct injector_t
    {
        /**
         * Binds the extracted member type to its index within the target reflection
         * type. This function does not aim to have its concrete return value used,
         * but only its return type.
         * @return The extracted type bound to the member index.
         */
        REFLECTOR_CONSTEXPR friend auto latch(tag_t<T, N>) noexcept
        {
            return typename std::remove_all_extents<P>::type {};
        }
    };

    /**
     * Decoy type responsible for pretending to be a type instance required to build
     * the target reflection type and latching the required type into the injector,
     * so that it can be retrieved later on.
     * @tparam T The target type for reflection processing.
     * @tparam N The index of property member type to extract.
     * @since 1.0
     */
    template <typename T, size_t N>
    struct decoy_t
    {
        /**
         * Injects the extracted member type into a latch if it has not yet been
         * previously done so.
         * @tparam P The extracted property member type.
         * @tparam M The index of the property member being processed.
         */
        template <typename P, size_t M>
        REFLECTOR_CONSTEXPR static auto inject(...) -> injector_t<T, P, M>;

      #if REFLECTOR_HOST_COMPILER != REFLECTOR_HOST_COMPILER_CLANG
        /**
         * Validates whether the type member being processed has already been reflected
         * over. If yes, avoids latch redeclaration.
         * @tparam M The index of the property member being processed.
         */
        template <typename, size_t M>
        REFLECTOR_CONSTEXPR static auto inject(int) -> decltype(latch(tag_t<T, M>()));
      #endif

        /**
         * Morphs the decoy into the required type for constructing the target
         * reflection type and injects it into the type latch.
         * @tparam P The type to morph the decoy into.
         */
        template <typename P, size_t = sizeof(inject<P, N>(0))>
        REFLECTOR_CONSTEXPR operator P&() const noexcept;
    };

    /**
     * The recursion base for counting the number of property members in the target
     * type of a reflection processing.
     * @tparam T The target type for reflection processing.
     * @return The total number of members within the target type.
     */
    template <typename T, size_t ...I>
    REFLECTOR_CONSTEXPR auto count(...) noexcept -> size_t
    {
        return sizeof...(I) - 1;
    }

    /**
     * Recursively counts the number of property members in the target type of a
     * reflection processing, by trying to construct the type with an increasing
     * number of parameters. If the current number of parameters succeeds, then it
     * is tested with one more parameter, until SFINAE.
     * @tparam T The target type for reflection processing.
     * @return The total number of members within the target type.
     */
    template <typename T, size_t ...I, size_t = sizeof(T{decoy_t<T, I>()...})>
    REFLECTOR_CONSTEXPR auto count(int) noexcept -> size_t
    {
        return count<T, I..., sizeof...(I)>(0);
    }

    /**
     * Extracts the types of the property members within the target reflection object
     * type into an instantiable tuple.
     * @tparam T The target type for reflection processing.
     * @return The tuple of extracted types.
     */
    template <typename T, size_t ...I, size_t = sizeof(T{decoy_t<T, I>()...})>
    REFLECTOR_CONSTEXPR auto loophole1(std::index_sequence<I...>) noexcept
    -> supertuple::tuple_t<decltype(latch(tag_t<T, I>()))...>;

    /**
     * Retrieves the tuple of extracted property member types of the given target
     * type to be reflected upon.
     * @tparam T The target type for reflecting upon.
     * @return The tuple of extracted types.
     */
    template <typename T>
    REFLECTOR_CONSTEXPR auto loophole() noexcept
    -> decltype(loophole1<T>(std::make_index_sequence<count<T>(0)>()));
}

REFLECTOR_END_NAMESPACE

REFLECTOR_DISABLE_GCC_WARNING_END("-Wnon-template-friend")
REFLECTOR_DISABLE_NVCC_WARNING_END(1301)

#endif
