/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_ACTION_CONCEPTS_HPP
#define RANGES_V3_ACTION_CONCEPTS_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/meta.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename T>
            struct movable_input_iterator
              : std::iterator<std::input_iterator_tag, T, std::ptrdiff_t, T *, T &&>
            {
                movable_input_iterator() = default;
                movable_input_iterator &operator++();
                movable_input_iterator operator++(int);
                bool operator==(movable_input_iterator const &) const;
                bool operator!=(movable_input_iterator const &) const;
                T && operator*() const;
            };
        }
        /// \endcond

        /// \addtogroup group-concepts
        /// @{
        namespace concepts
        {
            // std::array is a SemiContainer, native arrays are not.
            struct SemiContainer
              : refines<ForwardIterable>
            {
                template<typename T>
                auto requires_(T t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<DefaultConstructible, uncvref_t<T>>(),
                        concepts::model_of<Movable, uncvref_t<T>>(),
                        concepts::is_false(is_range<T>())
                    ));
            };

            // std::vector is a Container, std::array is not
            struct Container
              : refines<SemiContainer>
            {
                template<typename T, typename I = detail::movable_input_iterator<range_value_t<T>>>
                auto requires_(T t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Constructible, uncvref_t<T>, I, I>()
                    ));
            };
        }

        template<typename T>
        using SemiContainer = concepts::models<concepts::SemiContainer, T>;

        template<typename T>
        using Container = concepts::models<concepts::Container, T>;

        /// \cond
        namespace detail
        {
            template<typename T, CONCEPT_REQUIRES_(Container<T>())>
            std::true_type is_lvalue_container_like(T &);

            template<typename T, CONCEPT_REQUIRES_(Container<T>())>
            std::true_type is_lvalue_container_like(reference_wrapper<T>);

            template<typename T, CONCEPT_REQUIRES_(Container<T>())>
            std::true_type is_lvalue_container_like(std::reference_wrapper<T>);
        }
        /// \endcond

        namespace concepts
        {
            struct LvalueContainerLike
              : refines<ForwardIterable>
            {
                template<typename T>
                auto requires_(T t) -> decltype(
                    concepts::valid_expr(
                        detail::is_lvalue_container_like(std::forward<T>(t))
                    ));
            };
        }

        template<typename T>
        using LvalueContainerLike = concepts::models<concepts::LvalueContainerLike, T>;
        /// @}
    }
}

#endif
