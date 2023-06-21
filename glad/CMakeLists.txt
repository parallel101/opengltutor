add_library(glad OBJECT)
target_sources(glad PRIVATE src/glad.c include/glad/glad.h include/KHR/khrplatform.h)
target_include_directories(glad PUBLIC include)
