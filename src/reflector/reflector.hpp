/**
 * Reflector: A simple struct reflection framework for C++17.
 * @file The reflection framework implementation.
 * @author Rodrigo Siqueira <rodriados@gmail.com>
 * @copyright 2024-present Rodrigo Siqueira
 */
#pragma once

#include <utility>

#include <supertuple.hpp>

#include <reflector/environment.h>
#include <reflector/detail/loophole.hpp>

REFLECTOR_BEGIN_NAMESPACE

namespace supertuple = ::SUPERTUPLE_NAMESPACE;

namespace detail
{
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
            using reference_tuple_t = decltype(to_reference(std::declval<reflection_tuple_t>()));
            using storage_tuple_t = decltype(to_storage(std::declval<reflection_tuple_t>()));

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
