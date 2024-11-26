# cmake-format: off
# examples/CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# cmake-format: on

list(
  APPEND
  EXAMPLES
  modules
  playground
  sender-demo
  when_all-cancel
  stop_token
  stopping
  allocator
)

foreach(EXAMPLE ${EXAMPLES})
  set(EXAMPLE_TARGET ${TARGET_PREFIX}.examples.${EXAMPLE})
  add_executable(${EXAMPLE_TARGET})
  target_sources(${EXAMPLE_TARGET} PRIVATE ${EXAMPLE}.cpp)
  target_link_libraries(${EXAMPLE_TARGET} PRIVATE ${TARGET_LIBRARY})
endforeach()
