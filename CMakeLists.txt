cmake_minimum_required(VERSION 3.10)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 17)

project(main LANGUAGES CXX C)

if (UNIX AND NOT APPLE)
    find_package(X11 QUIET)
    if (X11_FOUND)
        link_libraries(${X11_LIBRARIES})
        include_directories(${X11_INCLUDE_DIR})
    endif()
    add_definitions(-DCMAKE_FOUND_X11)
endif()

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
