/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CORE_SCALE_SCALE_ENCODER_STREAM_HPP
#define CORE_SCALE_SCALE_ENCODER_STREAM_HPP

#include <deque>
#include <optional>

#include <gsl/span>
#include "scale/detail/fixed_witdh_integer.hpp"
#include "scale/detail/variant.hpp"

namespace scale {
  /**
   * @class ScaleEncoderStream designed to scale-encode data to stream
   */
  class ScaleEncoderStream {
   public:
    /// Getters
    /**
     * @return vector of bytes containing encoded data
     */
    std::vector<uint8_t> data() const;

    /**
     * @brief scale-encodes pair of values
     * @tparam F first value type
     * @tparam S second value type
     * @param p pair of values to encode
     * @return reference to stream
     */
    template <class F, class S>
    ScaleEncoderStream &operator<<(const std::pair<F, S> &p) {
      return *this << p.first << p.second;
    }

    /**
     * @brief scale-encodes variant value
     * @tparam T type list
     * @param v value to encode
     * @return reference to stream
     */
    template <class... T>
    ScaleEncoderStream &operator<<(const boost::variant<T...> &v) {
      detail::encodeVariant(v, *this);
      return *this;
    }

    /**
     * @brief scale-encodes collection of same time items
     * @tparam T type of item
     * @param c collection to encode
     * @return reference to stream
     */
    template <class T>
    ScaleEncoderStream &operator<<(const std::vector<T> &c) {
      return encodeCollection(c.size(), c.begin(), c.end());
    }

    /**
     * @brief scale-encodes optional value
     * @tparam T value type
     * @param v value to encode
     * @return reference to stream
     */
    template <class T>
    ScaleEncoderStream &operator<<(const std::optional<T> &v) {
      // optional bool is a special case of optional values
      // it should be encoded using one byte instead of two
      // as described in specification
      if constexpr (std::is_same<T, bool>::value) {
        return encodeOptionalBool(v);
      }
      if (!v.has_value()) {
        return putByte(0u);
      }
      return putByte(1u) << *v;
    }

    /**
     * @brief appends sequence of bytes
     * @param v bytes sequence
     * @return reference to stream
     */
    template <class T>
    ScaleEncoderStream &operator<<(const gsl::span<T> &v) {
      return encodeCollection(v.size(), v.begin(), v.end());
    }

    /**
     * @brief scale-encodes std::reference_wrapper of a type
     * @tparam T underlying type
     * @param v value to encode
     * @return reference to stream;
     */
    template <class T>
    ScaleEncoderStream &operator<<(const std::reference_wrapper<T> &v) {
      return *this << static_cast<const T &>(v);
    }

    /**
     * @brief scale-encodes a string view
     * @param sv string_view item
     * @return reference to stream
     */
    ScaleEncoderStream &operator<<(std::string_view sv) {
      return encodeCollection(sv.size(), sv.begin(), sv.end());
    }

    /**
     * @brief scale-encodes any integral type including bool
     * @tparam T integral type
     * @param v value of integral type
     * @return reference to stream
     */
    template <typename T, typename I = std::decay_t<T>,
              typename = std::enable_if_t<std::is_integral<I>::value>>
    ScaleEncoderStream &operator<<(T &&v) {
      // encode bool
      if constexpr (std::is_same<I, bool>::value) {
        uint8_t byte = (v ? 1u : 0u);
        return putByte(byte);
      }
      // put byte
      if constexpr (sizeof(T) == 1u) {
        // to avoid infinite recursion
        return putByte(static_cast<uint8_t>(v));
      }
      // encode any other integer
      detail::encodeInteger<I>(v, *this);
      return *this;
    }

    /**
     * @brief scale-encodes CompactInteger value as compact integer
     * @param v value to encode
     * @return reference to stream
     */
    ScaleEncoderStream &operator<<(const CompactInteger &v);

   protected:
    /**
     * @brief scale-encodes any collection
     * @tparam It iterator over collection of bytes
     * @param size size of the collection
     * @param begin iterator pointing to the begin of collection
     * @param end iterator pointing to the end of collection
     * @return reference to stream
     */
    template <class It>
    ScaleEncoderStream &encodeCollection(const CompactInteger &size, It &&begin,
                                         It &&end) {
      *this << size;
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      for (auto &&it = begin; it != end; ++it) {
        *this << *it;
      }
      return *this;
    }

    /// Appenders
    /**
     * @brief puts a byte to buffer
     * @param v byte value
     * @return reference to stream
     */
    ScaleEncoderStream &putByte(uint8_t v);

   private:
    ScaleEncoderStream &encodeOptionalBool(const std::optional<bool> &v);
    std::deque<uint8_t> stream_;
  };

}  // namespace scale

#endif  // CORE_SCALE_SCALE_ENCODER_STREAM_HPP
