# cmake-format: off
# examples/CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# cmake-format: on

set(BEMAN_EXECUTION26_LIBRARY beman_execution26)

include(GNUInstallDirs)

set(EXAMPLES
  stop_token
  stopped-only
)

foreach(EXAMPLE ${EXAMPLES})
  add_executable(${EXAMPLE})
  target_sources(${EXAMPLE} PRIVATE ${EXAMPLE}.cpp)
  target_link_libraries(${EXAMPLE} PRIVATE ${BEMAN_EXECUTION26_LIBRARY})
endforeach()
