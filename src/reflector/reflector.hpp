/**
 * Reflector: A simple struct reflection framework for C++17.
 * @file The reflection framework implementation.
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

/*
 * As we are exploiting some "obscure" behaviors of the language, and using some
 * tricks that might upset compilers, we need to disable some warnings in order to
 * force the compilation to take place without any warnings.
 */
REFLECTOR_DISABLE_NVCC_WARNING_BEGIN(1301)
REFLECTOR_DISABLE_GCC_WARNING_BEGIN("-Wnon-template-friend")

REFLECTOR_BEGIN_NAMESPACE

namespace supertuple = ::SUPERTUPLE_NAMESPACE;

/**
 * Reflects over the target data type, that is, it extracts information about the
 * member properties of the target type.
 * @tparam T The target data type to be introspected.
 * @since 1.0
 */
template <typename T>
class reflection_t;

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
     * A general aligned memory storage container.
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
    REFLECTOR_CONSTEXPR auto loophole(std::index_sequence<I...>) noexcept
    -> supertuple::tuple_t<decltype(latch(tag_t<T, I>()))...>;

    /**
     * Retrieves the tuple of extracted property member types of the given target
     * type to be reflected upon.
     * @tparam T The target type for reflecting upon.
     * @return The tuple of extracted types.
     */
    template <typename T>
    REFLECTOR_CONSTEXPR auto loophole() noexcept
    -> decltype(loophole<T>(std::make_index_sequence<count<T>(0)>()));

    /**
     * Transforms each member type of a tuple into its reference type.
     * @tparam T The tuple's type list.
     */
    template <typename ...T>
    REFLECTOR_CONSTEXPR auto to_ref_tuple(supertuple::tuple_t<T...>) noexcept
    -> supertuple::tuple_t<T&...>;

    /**
     * Transforms each member type of a tuple into an aligned storage.
     * @tparam T The tuple's type list.
     */
    template <typename ...T>
    REFLECTOR_CONSTEXPR auto to_storage_tuple(supertuple::tuple_t<T...>) noexcept
    -> supertuple::tuple_t<storage_t<sizeof(T), alignof(T)>...>;

    /**
     * Reflects over a trivial object type and extracts information about its internal
     * property members, transforming it into instantiable tuples.
     * @tparam T The type to be analyzed.
     * @since 1.0
     */
    template <typename T>
    class reflector_t
    {
        static_assert(!std::is_union<T>::value, "union types cannot be reflected");
        static_assert(std::is_trivial<T>::value, "reflected type must be trivial");

        private:
            template <typename A, typename B>
            using is_compatible = std::bool_constant<sizeof(A) == sizeof(B) && alignof(A) == alignof(B)>;

        public:
            using reflection_tuple_t = decltype(loophole<T>());
            using reference_tuple_t = decltype(to_ref_tuple(std::declval<reflection_tuple_t>()));
            using storage_tuple_t = decltype(to_storage_tuple(std::declval<reflection_tuple_t>()));

        static_assert(
            is_compatible<reflection_tuple_t, T>::value
          , "reflection tuple is not compatible with target type");
    };
}

/**
 * Extracts and manages references to each member property of the target type, thus
 * enumerating each of the target type's property members and allowing them to be
 * directly accessed or updated.
 * @tparam T The target data type to be introspected.
 * @since 1.0
 */
template <typename T>
class reflection_t : public detail::reflector_t<T>::reference_tuple_t
{
    private:
        typedef detail::reflector_t<T> reflector_t;
        typedef typename reflector_t::reference_tuple_t underlying_t;

    public:
        using reference_tuple_t = underlying_t;
        using reflection_tuple_t = typename reflector_t::reflection_tuple_t;

    public:
        REFLECTOR_INLINE reflection_t() noexcept = delete;
        REFLECTOR_INLINE reflection_t(const reflection_t&) noexcept = default;
        REFLECTOR_INLINE reflection_t(reflection_t&&) noexcept = default;

        /**
         * Reflects over an instance and gathers refereces to its members.
         * @param target The target instance to get references from.
         */
        REFLECTOR_INLINE reflection_t(T& target) noexcept
          : underlying_t (extract(target, std::make_index_sequence<underlying_t::count>()))
        {}

        REFLECTOR_INLINE reflection_t& operator=(const reflection_t&) = default;
        REFLECTOR_INLINE reflection_t& operator=(reflection_t&&) = default;

    public:
        /**
         * Retrieves the offset of a member of the reflected type by its index.
         * @tparam N The index of required member.
         * @return The member offset.
         */
        template <size_t N>
        REFLECTOR_CONSTEXPR static auto offset() noexcept -> ptrdiff_t
        {
            constexpr typename reflector_t::storage_tuple_t s {};
            return reinterpret_cast<size_t>(&s.template get<N>())
                -  reinterpret_cast<size_t>(&s.template get<0>());
        }

        /**
         * Retrieves a property member reference from an instance by its index.
         * @tparam N The requested property member index.
         * @param target The target instance to retrieve member reference from.
         * @return The extracted member reference.
         */
        template <size_t N>
        REFLECTOR_CONSTEXPR static auto member(T& target) noexcept
        -> typename reference_tuple_t::template element_t<N> {
            using E = typename reflection_tuple_t::template element_t<N>;
            return *reinterpret_cast<E*>(reinterpret_cast<uint8_t*>(&target) + offset<N>());
        }

    private:
        /**
         * Retrieves references to the properties of a reflected instance.
         * @tparam I The member types index sequence.
         * @param target The reflected type instance to gather references from.
         * @return The new reference tuple instance.
         */
        template <size_t ...I>
        REFLECTOR_INLINE static underlying_t extract(T& target, std::index_sequence<I...>) noexcept
        {
            return underlying_t(member<I>(target)...);
        }
};

REFLECTOR_END_NAMESPACE

/**
 * Informs the size of a generic reflection tuple, allowing it to be deconstructed.
 * @tparam T The target type for reflection.
 * @since 1.0
 */
template <typename T>
struct std::tuple_size<REFLECTOR_NAMESPACE::reflection_t<T>>
  : std::integral_constant<size_t, REFLECTOR_NAMESPACE::reflection_t<T>::count> {};

/**
 * Retrieves the deconstruction type of a reflection tuple's element.
 * @tparam I The index of the requested tuple element.
 * @tparam T The target type for reflection.
 * @since 1.0
 */
template <size_t I, typename T>
struct std::tuple_element<I, REFLECTOR_NAMESPACE::reflection_t<T>> {
    using type = typename REFLECTOR_NAMESPACE::reflection_t<T>::template element_t<I>;
};

REFLECTOR_DISABLE_GCC_WARNING_END("-Wnon-template-friend")
REFLECTOR_DISABLE_NVCC_WARNING_END(1301)
