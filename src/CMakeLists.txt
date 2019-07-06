#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

add_subdirectory(common)

add_library(scale
    scale_decoder_stream.cpp
    scale_encoder_stream.cpp
    scale_error.cpp
    )

target_link_libraries(scale
    Boost::boost
    scale_buffer
    )
