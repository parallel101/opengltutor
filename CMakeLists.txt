cmake_minimum_required(VERSION 3.18)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 20)

project(main LANGUAGES CXX)

add_executable(main main.cpp)

add_subdirectory(glm)
target_link_libraries(main PUBLIC glm)
add_subdirectory(glfw)
target_link_libraries(main PUBLIC glfw)

find_package(OpenGL REQUIRED)
target_include_directories(main PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(main PUBLIC ${OPENGL_LIBRARIES})
