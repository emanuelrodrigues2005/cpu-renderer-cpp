cmake_minimum_required(VERSION 3.16)

project(cpu-renderer-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)

add_executable(cpu-renderer
    src/main.cpp
    src/WindowSDL.cpp
)

if(TARGET SDL2::SDL2)
    target_link_libraries(cpu-renderer PRIVATE SDL2::SDL2)
else()
    target_include_directories(cpu-renderer PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(cpu-renderer PRIVATE ${SDL2_LIBRARIES})
endif()

target_compile_options(cpu-renderer PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()

add_custom_target(run
    COMMAND cpu-renderer
    DEPENDS cpu-renderer
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    USES_TERMINAL
)

add_custom_target(smoke
    COMMAND ${CMAKE_COMMAND} -E env SDL_VIDEODRIVER=dummy
            $<TARGET_FILE:cpu-renderer> --smoke
    DEPENDS cpu-renderer
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    USES_TERMINAL
)

add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS cpu-renderer
    USES_TERMINAL
)
