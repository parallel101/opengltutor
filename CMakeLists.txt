cmake_minimum_required(VERSION 3.10)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(opengltutor
    LANGUAGES CXX C
    DESCRIPTION "An indispensable online webinar open course for computer graphics beginners to learn every bit of modern OpenGL and build a popular 3D game engine."
    HOMEPAGE_URL https://github.com/parallel101/opengltutor
    )

if (PROJECT_BINARY_DIR STREQUAL PROJECT_SOURCE_DIR)
    message(FATAL_ERROR "The binary directory of CMake cannot be the same as source directory! "
        "Please consider use 'cmake -B build' to specify a different binary directory. "
        "Otherwise this project may fail to build, or make Git hard to exclude binary files. "
        "For Windows, it is highly recommended to use Visual Studio as IDE for CMake projects.")
endif()

if (UNIX AND NOT APPLE)
    find_package(X11 QUIET)
    if (X11_FOUND)
        link_libraries(${X11_LIBRARIES})
        include_directories(${X11_INCLUDE_DIR})
    endif()
    add_definitions(-DCMAKE_FOUND_X11)
endif()

if (NOT MSVC)
    find_program(CCACHE_PROGRAM ccache)  # See https://ccache.dev
    if (CCACHE_PROGRAM)
        message(STATUS "Found CCache: ${CCACHE_PROGRAM}")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PROGRAM})
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_PROGRAM})
    endif()
endif()

if (MSVC)
    add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/utf-8$<SEMICOLON>/DNOMINMAX$<SEMICOLON>/D_USE_MATH_DEFINES>)
    add_compile_options($<$<COMPILE_LANG_AND_ID:C,MSVC>:/utf-8$<SEMICOLON>/DNOMINMAX$<SEMICOLON>/D_USE_MATH_DEFINES>)
endif()

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE headers CONFIGURE_DEPENDS include/*.h include/*.hpp)
add_executable(${PROJECT_NAME} ${sources} ${headers})
target_include_directories(${PROJECT_NAME} PUBLIC include)

add_subdirectory(glm)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)
add_subdirectory(glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
add_subdirectory(glad)
target_link_libraries(${PROJECT_NAME} PUBLIC glad)
