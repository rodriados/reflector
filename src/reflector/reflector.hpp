/**
 * Reflector: A simple struct reflection framework for C++17.
 * @file The reflection framework implementation.
 * @author Rodrigo Siqueira <rodriados@gmail.com>
 * @copyright 2024-present Rodrigo Siqueira
 */
#pragma once

#include <utility>

#include <reflector/environment.h>
#include <reflector/provider.hpp>

REFLECTOR_BEGIN_NAMESPACE

/**
 * Extracts and manages references to each member property of the target type, thus
 * enumerating each of the target type's property members and allowing them to be
 * directly accessed or updated.
 * @tparam T The target data type to be introspected.
 * @since 1.0
 */
template <typename T>
class reflection_t : public decltype(provider_t<T>::provide())::reference_tuple_t
{
    public:
        typedef T target_t;

    private:
        typedef decltype(provider_t<T>::provide()) descriptor_t;
        typedef typename descriptor_t::reference_tuple_t underlying_t;

    static_assert(
        std::is_same_v<T, typename descriptor_t::target_t>
      , "the reflection target type is not the same as the descriptor type");

    public:
        using reference_tuple_t = underlying_t;
        using reflection_tuple_t = typename descriptor_t::reflection_tuple_t;

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
            constexpr typename descriptor_t::storage_tuple_t s {};
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
