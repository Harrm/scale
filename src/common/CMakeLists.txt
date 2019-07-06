add_library(scale_hexutil
  hexutil.hpp
  hexutil.cpp
  )
target_link_libraries(scale_hexutil
  Boost::boost
  )

add_library(scale_buffer
  buffer.hpp
  buffer.cpp
  buffer_back_insert_iterator.cpp
  )
target_link_libraries(scale_buffer
    scale_hexutil
  )

message(STATUS "BBBBBBBBBB ${INCLUDE_DIRECTORIES}")
