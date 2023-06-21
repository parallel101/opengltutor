cmake_minimum_required(VERSION 3.18)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 14)

project(main LANGUAGES CXX C)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE headers CONFIGURE_DEPENDS include/*.h include/*.hpp)
add_executable(main ${sources} ${headers})
target_include_directories(main PUBLIC include)

add_subdirectory(glm)
target_link_libraries(main PUBLIC glm)
add_subdirectory(glfw)
target_link_libraries(main PUBLIC glfw)
add_subdirectory(glad)
target_link_libraries(main PUBLIC glad)
