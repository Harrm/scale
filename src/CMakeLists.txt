#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

add_subdirectory(scale/common)

add_library(scale
    scale/scale_decoder_stream.cpp
    scale/scale_encoder_stream.cpp
    scale/scale_error.cpp
    )

target_include_directories(scale PUBLIC
    SYSTEM
    # system includes
    ${PROJECT_SOURCE_DIR}/deps/gsl/include
    ${PROJECT_SOURCE_DIR}/deps/outcome
)

target_include_directories(scale PUBLIC
    # project includes
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(scale
    Boost::boost
    scale_buffer
    )
