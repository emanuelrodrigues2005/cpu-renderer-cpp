cmake_minimum_required(VERSION 3.16)

project(cpu-renderer-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)

add_executable(cpu-renderer
    src/main.cpp
    src/Mesh.cpp
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

add_executable(test_mesh
    tests/test_mesh.cpp
    src/Mesh.cpp
)

target_include_directories(test_mesh PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_mesh PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME mesh COMMAND test_mesh)
set_tests_properties(mesh PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(test_vec3
    tests/test_vec3.cpp
    src/Vec3.cpp
)

target_include_directories(test_vec3 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_vec3 PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME vec3 COMMAND test_vec3)

add_test(NAME info_sucesso COMMAND cpu-renderer --info models/triangulo.byu)
set_tests_properties(info_sucesso PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    PASS_REGULAR_EXPRESSION "models/triangulo.byu: 3 vértices, 1 triângulo"
    TIMEOUT 10
)

add_test(NAME info_arquivo_inexistente COMMAND cpu-renderer --info models/inexistente.byu)
set_tests_properties(info_arquivo_inexistente PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    WILL_FAIL TRUE
    TIMEOUT 10
)

add_test(NAME smoke COMMAND ${CMAKE_COMMAND} -E env SDL_VIDEODRIVER=dummy
                            $<TARGET_FILE:cpu-renderer> --smoke)
set_tests_properties(smoke PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    TIMEOUT 10
)

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
    DEPENDS cpu-renderer test_mesh test_vec3
    USES_TERMINAL
)
