/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_VIEW_BOUNDED_HPP
#define RANGES_V3_VIEW_BOUNDED_HPP

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_interface.hpp>
#include <range/v3/utility/pipeable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/common_iterator.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{

        template<typename Rng>
        struct bounded_view
          : range_interface<bounded_view<Rng>, is_infinite<Rng>::value>
        {
        private:
            friend range_access;
            using base_range_t = view::all_t<Rng>;
            using base_iterator_t = range_iterator_t<base_range_t>;
            using base_sentinel_t = range_sentinel_t<base_range_t>;
            base_range_t rng_;

        public:
            using iterator = common_iterator<base_iterator_t, base_sentinel_t>;

            bounded_view() = default;
            explicit bounded_view(Rng && rng)
              : rng_(view::all(std::forward<Rng>(rng)))
            {}
            iterator begin()
            {
                return iterator{ranges::begin(rng_)};
            }
            iterator end()
            {
                return iterator{ranges::end(rng_)};
            }
            CONCEPT_REQUIRES(Range<base_range_t const>())
            iterator begin() const
            {
                return iterator{ranges::begin(rng_)};
            }
            CONCEPT_REQUIRES(Range<base_range_t const>())
            iterator end() const
            {
                return iterator{ranges::end(rng_)};
            }
            CONCEPT_REQUIRES(SizedRange<base_range_t>())
            range_size_t<base_range_t> size() const
            {
                return ranges::size(rng_);
            }
            base_range_t & base()
            {
                return rng_;
            }
            base_range_t const & base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            struct bounded_fn
            {
                template<typename Rng,
                    CONCEPT_REQUIRES_(Iterable<Rng>() && !BoundedIterable<Rng>())>
                bounded_view<Rng> operator()(Rng && rng) const
                {
                    return bounded_view<Rng>{std::forward<Rng>(rng)};
                }
                template<typename Rng,
                    CONCEPT_REQUIRES_(Iterable<Rng>() && BoundedIterable<Rng>())>
                ranges::view::all_t<Rng> operator()(Rng && rng) const
                {
                    return ranges::view::all(std::forward<Rng>(rng));
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Iterable<Rng>())>
                void operator()(Rng && rng) const
                {
                    CONCEPT_ASSERT_MSG(Iterable<Rng>(),
                        "Rng is not a model of the Iterable concept");
                }
            #endif
            };

            /// \relates bounded_fn
            /// \ingroup group-views
            constexpr view<bounded_fn> bounded{};

            template<typename Rng>
            using bounded_t =
                decltype(bounded(std::declval<Rng>()));
        }
        /// @}
    }
}

#endif
