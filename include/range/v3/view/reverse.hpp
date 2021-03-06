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

#ifndef RANGES_V3_VIEW_REVERSE_HPP
#define RANGES_V3_VIEW_REVERSE_HPP

#include <utility>
#include <iterator>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct reverse_view
          : range_adaptor<reverse_view<Rng>, Rng>
        {
        private:
            CONCEPT_ASSERT(BidirectionalIterable<Rng>());
            CONCEPT_ASSERT(BoundedIterable<Rng>());
            friend range_access;

            // A rather convoluted implementation to avoid the problem std::reverse_iterator
            // has adapting iterators that return references to internal data.
            struct adaptor : adaptor_base
            {
            private:
                reverse_view const *rng_;
            public:
                adaptor() = default;
                adaptor(reverse_view const &rng)
                  : rng_(&rng)
                {}
                range_iterator_t<Rng> begin(reverse_view const &rng) const
                {
                    auto it = ranges::end(rng.mutable_base());
                    ranges::advance_bounded(it, -1, ranges::begin(rng.mutable_base()));
                    return it;
                }
                void next(range_iterator_t<Rng> &it) const
                {
                    if(0 != ranges::advance_bounded(it, -1, ranges::begin(rng_->mutable_base())))
                        it = ranges::end(rng_->mutable_base());
                }
                void prev(range_iterator_t<Rng> &it) const
                {
                    if(0 != ranges::advance_bounded(it, 1, ranges::end(rng_->mutable_base())))
                        it = ranges::begin(rng_->mutable_base());
                }
                CONCEPT_REQUIRES(RandomAccessIterable<Rng>())
                void advance(range_iterator_t<Rng> &it, range_difference_t<Rng> n) const
                {
                    if(n > 0)
                        ranges::advance(it, -n + 1), this->next(it);
                    else if(n < 0)
                        this->prev(it), ranges::advance(it, -n - 1);
                }
                CONCEPT_REQUIRES(RandomAccessIterable<Rng>())
                range_difference_t<Rng>
                distance_to(range_iterator_t<Rng> const &here, range_iterator_t<Rng> const &there,
                    adaptor const &other_adapt) const
                {
                    RANGES_ASSERT(rng_ == other_adapt.rng_);
                    if(there == ranges::end(rng_->mutable_base()))
                        return here == ranges::end(rng_->mutable_base())
                            ? 0 : (here - ranges::begin(rng_->mutable_base())) + 1;
                    else if(here == ranges::end(rng_->mutable_base()))
                        return (ranges::begin(rng_->mutable_base()) - there) - 1;
                    return here - there;
                }
            };
            adaptor begin_adaptor() const
            {
                return {*this};
            }
            adaptor end_adaptor() const
            {
                return {*this};
            }
        public:
            reverse_view() = default;
            reverse_view(Rng && rng)
              : range_adaptor_t<reverse_view>{std::forward<Rng>(rng)}
            {}
            CONCEPT_REQUIRES(SizedIterable<Rng>())
            range_size_t<Rng> size() const
            {
                return ranges::size(this->base());
            }
        };

        namespace view
        {
            struct reverse_fn
            {
                template<typename Rng>
                using Concept = meta::and_<
                    BidirectionalIterable<Rng>,
                    BoundedIterable<Rng>>;

                template<typename Rng, CONCEPT_REQUIRES_(Concept<Rng>())>
                reverse_view<Rng> operator()(Rng && rng) const
                {
                    return reverse_view<Rng>{std::forward<Rng>(rng)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                // For error reporting
                template<typename Rng, CONCEPT_REQUIRES_(!Concept<Rng>())>
                void operator()(Rng &&) const
                {
                    CONCEPT_ASSERT_MSG(BidirectionalIterable<Rng>(),
                        "The object on which view::reverse operates must be a model of the "
                        "BidirectionalIterable concept.");
                    CONCEPT_ASSERT_MSG(BoundedIterable<Rng>(),
                        "To reverse an iterable object, its end iterator must be a model of "
                        "the BidirectionalIterator concept.");
                }
            #endif
            };

            /// \relates reverse_fn
            /// \ingroup group-views
            constexpr view<reverse_fn> reverse{};
        }
        /// @}
    }
}

#endif
