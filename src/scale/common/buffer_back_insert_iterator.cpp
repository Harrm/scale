/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "scale/common/buffer.hpp"

using scale::common::Buffer;

class value_type;
namespace std {

/*
 * std::back_insert_iterator is an output iterator
 * that appends to a container for which it was constructed.
 */
template <>
class back_insert_iterator<Buffer> {
public:
  using value_type = Buffer::value_type;

  constexpr explicit back_insert_iterator(Buffer &c):
      buf_ {c} {

  }

  back_insert_iterator<Buffer>&
  operator=(uint8_t value) {
    buf_.putUint8(value);
    return *this;
  }

  back_insert_iterator<Buffer>&
  operator=(uint32_t value) {
    buf_.putUint32(value);
    return *this;
  }

  back_insert_iterator<Buffer>&
  operator=(uint64_t value) {
    buf_.putUint64(value);
    return *this;
  }

  back_insert_iterator<Buffer>&
  operator=(std::string_view value) {
    buf_.put(value);
    return *this;
  }

  back_insert_iterator<Buffer>&
  operator=(gsl::span<const uint8_t> s) {
    buf_.put(s);
    return *this;
  }

  back_insert_iterator<Buffer>&
  operator=(const std::vector<uint8_t>& v) {
    buf_.put(v);
    return *this;
  }

  constexpr back_insert_iterator& operator*() {
    return *this;
  }

  constexpr back_insert_iterator& operator++() {
    return *this;
  }

  constexpr back_insert_iterator& operator++(int) {
    return *this;
  }

private:
  Buffer& buf_;
};

}
