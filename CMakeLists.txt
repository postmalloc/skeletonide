cmake_minimum_required(VERSION 3.12)
project(skeletonide)
include_directories(include/third_party)
add_executable(skelgen src/pipeline.cpp)
target_link_libraries(skelgen Halide pthread dl png16 jpeg)
add_custom_command(
        TARGET skelgen
        COMMAND skelgen
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_executable(spook spook.cpp)
add_dependencies(spook skelgen)
link_directories(${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(spook ${CMAKE_CURRENT_BINARY_DIR}/skeletonide.a pthread dl png16 jpeg )
add_custom_command(
        TARGET spook
        COMMAND mv spook ${CMAKE_HOME_DIRECTORY}
)