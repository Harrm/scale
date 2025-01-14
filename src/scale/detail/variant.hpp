/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCALE_VARIANT_HPP
#define SCALE_VARIANT_HPP

#include <boost/variant.hpp>
#include <outcome/outcome.hpp>
#include "scale/scale_error.hpp"
#include "scale/outcome_throw.hpp"
#include "scale/common/visitor.hpp"

namespace scale::detail {
  namespace impl {
    template <uint8_t i, class F, class H, class... T>
    void for_each_apply_impl(F &f);

    template <class F, class H>
    void apply_once(F &f, uint8_t i) {
      f.template apply<H>(i);
    }

    template <uint8_t i, class F>
    void for_each_apply_impl(F &f) {
      // do nothing, end of recursion
    }

    template <uint8_t i, class F, class H, class... T>
    void for_each_apply_impl(F &f) {
      apply_once<F, H>(f, i);
      for_each_apply_impl<i + 1, F, T...>(f);
    }

    template <class Stream, class... T>
    class VariantEncoder {
     public:
      VariantEncoder(const boost::variant<T...> &v, Stream &s) : v_{v}, s_{s} {}

      template <class H>
      void apply(uint8_t index) {
        // if type matches alternative in variant then encode
        scale::visit_in_place(
            v_,
            [this, index](const H &h) {
              s_ << index << h;  // first byte means type index
            },
            [](const auto & /*unused*/) {});
      }

     private:
      const boost::variant<T...> &v_;
      Stream &s_;
    };

    template <class Stream, class... T>
    class VariantDecoder {
     public:
      VariantDecoder(uint8_t type_index, boost::variant<T...> &r, Stream &s)
          : type_index_{type_index}, r_{r}, s_{s} {};

      template <class H>
      void apply(uint8_t index) {
        if (index == type_index_) {
          H h{};
          s_ >> h;  // decode custom type
          r_ = std::move(h);
        }
      }

     private:
      uint8_t type_index_;
      boost::variant<T...> &r_;
      Stream &s_;
    };

    template <class F, class... T>
    void for_each_apply(F &f) {
      impl::for_each_apply_impl<0, F, T...>(f);
    }
  }  // namespace impl

  /**
   * @brief encodes boost::variant value
   * @tparam Stream output stream type
   * @tparam T... sequence of types
   * @param v variant value
   * @param s encoder stream
   */
  template <class Stream, class... T>
  void encodeVariant(const boost::variant<T...> &v, Stream &s) {
    auto &&encoder = impl::VariantEncoder(v, s);
    impl::for_each_apply<decltype(encoder), T...>(encoder);
  }

  /**
   * @brief decodes variant value
   * @tparam Stream input stream type
   * @tparam T... sequence of types
   * @param stream source stream
   * @return decoded value or error
   */
  template <class Stream, class... T>
  Stream &decodeVariant(Stream &stream, boost::variant<T...> &result) {
    // first byte means type index
    uint8_t type_index = 0u;
    stream >> type_index;  // decode type index
    static constexpr uint8_t types_count = sizeof...(T);
    // ensure that index is in [0, types_count)
    if (type_index >= types_count) {
      common::raise(DecodeError::WRONG_TYPE_INDEX);
    }

    auto &&decoder = impl::VariantDecoder(type_index, result, stream);
    impl::for_each_apply<decltype(decoder), T...>(decoder);

    return stream;
  }

}  // namespace scale::detail

#endif  // SCALE_VARIANT_HPP
