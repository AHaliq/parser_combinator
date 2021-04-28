file(GLOB srcs
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp"
)
file(GLOB headers
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.h"
)
add_library(src ${srcs} ${headers})

add_executable(parser-combinator ${srcs})