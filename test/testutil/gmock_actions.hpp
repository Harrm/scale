
/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GMOCK_ACTIONS_HPP
#define GMOCK_ACTIONS_HPP

#include <gmock/gmock.h>
#include <boost/system/error_code.hpp>

/**
 * @code
 * const int size = 1;
 * EXPECT_CALL(*connection_, read(_, _, _)).WillOnce(AsioSuccess(size));
 * auto buf = std::make_shared<std::vector<uint8_t>>(size, 0);
 * secure_connection_->read(*buf, size, [&size, buf](auto &&res) mutable {
 *   ASSERT_TRUE(res) << res.error().message();
 *   ASSERT_EQ(read, size);
 * });
 * @nocode
 */
ACTION_P(AsioSuccess, size) {
  // arg0 - buffer
  // arg1 - bytes
  // arg2 - callback
  arg2(size);
}

/**
 * @code
 * const int size = 1;
 * boost::system::error_code ec = ...;
 * EXPECT_CALL(*connection_, read(_, _, _)).WillOnce(AsioCallback(ec, size));
 * auto buf = std::make_shared<std::vector<uint8_t>>(size, 0);
 * secure_connection_->read(*buf, size, [&size, buf, e=ec](auto &&ec, size_t
 * read) mutable { ASSERT_EQ(ec.value(), e.value()); ASSERT_EQ(read, size);
 * });
 * @nocode
 */
ACTION_P2(AsioCallback, ec, size) {
  // arg0 - buffer
  // arg1 - bytes
  // arg2 - callback
  arg2(ec, size);
}

ACTION_P(Arg0CallbackWithArg, in) {
  arg0(in);
}

ACTION_P(Arg1CallbackWithArg, in) {
  arg1(in);
}

ACTION_P(Arg2CallbackWithArg, in) {
  arg2(in);
}

ACTION_P(Arg3CallbackWithArg, in) {
  arg3(in);
}

#endif  // GMOCK_ACTIONS_HPP
