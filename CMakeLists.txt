cmake_minimum_required(VERSION 3.10)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(opengltutor
    LANGUAGES CXX C
    DESCRIPTION "An indispensable online webinar open course for computer graphics beginners to learn every bit of modern OpenGL and build a popular 3D game engine."
    HOMEPAGE_URL https://github.com/parallel101/opengltutor
    )

if (PROJECT_BINARY_DIR STREQUAL PROJECT_SOURCE_DIR)
    message(FATAL_ERROR "The binary directory of CMake cannot be the same as source directory! "
        "Do not use 'cmake .'! Please consider use 'cmake -B build' to specify a different binary directory. "
        "Otherwise this project may fail to build, or make Git hard to exclude binary files. "
        "For Windows, it is highly recommended to use Visual Studio as IDE for CMake projects.")
endif()

message(STATUS "CMake build type: ${CMAKE_BUILD_TYPE}")

if (NOT MSVC)
    find_program(CCACHE_PROGRAM ccache)  # Compilation speed up: https://ccache.dev
    if (CCACHE_PROGRAM)
        message(STATUS "Found CCache: ${CCACHE_PROGRAM}")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PROGRAM})
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_PROGRAM})
    endif()
endif()

if (MSVC)  # 1. fix string literial cannot include unicode characters; 2. rid min and max macros; 3. get back M_PI, M_E macros
    add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/utf-8$<SEMICOLON>/DNOMINMAX$<SEMICOLON>/D_USE_MATH_DEFINES>)
    add_compile_options($<$<COMPILE_LANG_AND_ID:C,MSVC>:/utf-8$<SEMICOLON>/DNOMINMAX$<SEMICOLON>/D_USE_MATH_DEFINES>)
elseif (CMAKE_COMPILER_IS_GNUCXX)  # Add necessary checks and warnings
    if (CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
        add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wall$<SEMICOLON>-Wextra>)
        add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-ftrapv$<SEMICOLON>-fbounds-check$<SEMICOLON>-fsanitize-trap>)
    endif()
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

target_compile_definitions(${PROJECT_NAME} PUBLIC -DOPENGLTUTOR_HOME=\"${CMAKE_CURRENT_SOURCE_DIR}/\")
