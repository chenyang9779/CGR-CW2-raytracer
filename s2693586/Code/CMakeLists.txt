cmake_minimum_required(VERSION 3.10)
project(RayTracer)

set(CMAKE_CXX_STANDARD 17)

# Add all source files
add_executable(main main.cpp camera/camera.cpp geometry/geometry.cpp)

# Link any necessary libraries (add any additional required libraries)
target_link_libraries(main PRIVATE jsoncpp)
