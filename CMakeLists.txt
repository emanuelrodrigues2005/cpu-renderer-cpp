cmake_minimum_required(VERSION 3.16)

project(cpu-renderer-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)

add_executable(cpu-renderer
    src/main.cpp
    src/mesh/Mesh.cpp
    src/math/Vec3.cpp
    src/pipeline/Camera.cpp
    src/pipeline/Projection.cpp
    src/pipeline/Rasterizer.cpp
    src/pipeline/Renderer.cpp
    src/canvas/WindowSDL.cpp
    src/canvas/MemoryCanvas.cpp
    src/canvas/BmpWriter.cpp
)

if(TARGET SDL2::SDL2)
    target_link_libraries(cpu-renderer PRIVATE SDL2::SDL2)
else()
    target_include_directories(cpu-renderer PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(cpu-renderer PRIVATE ${SDL2_LIBRARIES})
endif()

target_include_directories(cpu-renderer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(cpu-renderer PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()

add_executable(test_mesh
    tests/test_mesh.cpp
    src/mesh/Mesh.cpp
)

target_include_directories(test_mesh PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_mesh PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME mesh COMMAND test_mesh)
set_tests_properties(mesh PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(test_vec3
    tests/test_vec3.cpp
    src/math/Vec3.cpp
)

target_include_directories(test_vec3 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_vec3 PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME vec3 COMMAND test_vec3)

add_executable(test_camera
    tests/test_camera.cpp
    src/pipeline/Camera.cpp
    src/math/Vec3.cpp
)

target_include_directories(test_camera PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_camera PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME camera COMMAND test_camera)
set_tests_properties(camera PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(test_projection
    tests/test_projection.cpp
    src/pipeline/Projection.cpp
    src/pipeline/Camera.cpp
    src/math/Vec3.cpp
)

target_include_directories(test_projection PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_projection PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME projection COMMAND test_projection)
set_tests_properties(projection PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(test_rasterizer
    tests/test_rasterizer.cpp
    src/pipeline/Rasterizer.cpp
    src/canvas/MemoryCanvas.cpp
)

target_include_directories(test_rasterizer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_rasterizer PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME rasterizer COMMAND test_rasterizer)

add_executable(test_renderer
    tests/test_renderer.cpp
    src/pipeline/Renderer.cpp
    src/pipeline/Rasterizer.cpp
    src/pipeline/Projection.cpp
    src/pipeline/Camera.cpp
    src/math/Vec3.cpp
    src/mesh/Mesh.cpp
    src/canvas/MemoryCanvas.cpp
)

target_include_directories(test_renderer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_renderer PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME renderer COMMAND test_renderer)
set_tests_properties(renderer PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(test_bmp_writer
    tests/test_bmp_writer.cpp
    src/canvas/BmpWriter.cpp
    src/canvas/MemoryCanvas.cpp
)

target_include_directories(test_bmp_writer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_bmp_writer PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME bmp_writer COMMAND test_bmp_writer)

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

add_test(NAME dump_sucesso COMMAND cpu-renderer --dump ${CMAKE_BINARY_DIR}/dump-teste.bmp
                                      models/triangulo.byu camera/camera.txt)
set_tests_properties(dump_sucesso PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    PASS_REGULAR_EXPRESSION "BMP salvo"
    TIMEOUT 10
)

add_test(NAME dump_modelo_inexistente COMMAND cpu-renderer --dump
                                             ${CMAKE_BINARY_DIR}/dump-erro.bmp
                                             models/inexistente.byu camera/camera.txt)
set_tests_properties(dump_modelo_inexistente PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    WILL_FAIL TRUE
    TIMEOUT 10
)

add_test(NAME sdl_backend_policy
         COMMAND ${CMAKE_COMMAND}
                 -DPROJECT_SOURCE_DIR=${CMAKE_SOURCE_DIR}
                 -P ${CMAKE_SOURCE_DIR}/tests/check_sdl_backend.cmake)
set_tests_properties(sdl_backend_policy PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    TIMEOUT 10
)

add_test(NAME default_render_mode
         COMMAND ${CMAKE_COMMAND}
                 -DPROJECT_SOURCE_DIR=${CMAKE_SOURCE_DIR}
                 -P ${CMAKE_SOURCE_DIR}/tests/check_default_mode.cmake)
set_tests_properties(default_render_mode PROPERTIES
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
    DEPENDS cpu-renderer test_mesh test_vec3 test_camera test_projection test_rasterizer test_renderer test_bmp_writer
    USES_TERMINAL
)
