file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_library(test ${srcs} ${headers})

include_directories(../lib)
include_directories(../src)
add_executable(tests ${srcs})

target_link_libraries(test src)