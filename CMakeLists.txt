cmake_minimum_required(VERSION 3.16)

project(cpu-renderer-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)

add_executable(cpu-renderer
    src/main.cpp
    src/mesh/Mesh.cpp
    src/mesh/Normals.cpp
    src/math/Vec3.cpp
    src/math/Color.cpp
    src/pipeline/Camera.cpp
    src/pipeline/Projection.cpp
    src/pipeline/DepthBuffer.cpp
    src/pipeline/Rasterizer.cpp
    src/pipeline/ShadedRasterizer.cpp
    src/pipeline/Renderer.cpp
    src/shading/Lighting.cpp
    src/shading/Phong.cpp
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

add_executable(test_depth_buffer
    tests/test_depth_buffer.cpp
    src/pipeline/DepthBuffer.cpp
)

target_include_directories(test_depth_buffer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_depth_buffer PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME depth_buffer COMMAND test_depth_buffer)

add_executable(test_rasterizer
    tests/test_rasterizer.cpp
    src/pipeline/Rasterizer.cpp
    src/canvas/MemoryCanvas.cpp
)

target_include_directories(test_rasterizer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_rasterizer PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME rasterizer COMMAND test_rasterizer)

add_executable(test_shaded_rasterizer
    tests/test_shaded_rasterizer.cpp
    src/pipeline/ShadedRasterizer.cpp
    src/pipeline/DepthBuffer.cpp
    src/pipeline/Rasterizer.cpp
    src/shading/Phong.cpp
    src/shading/Lighting.cpp
    src/math/Color.cpp
    src/math/Vec3.cpp
    src/canvas/MemoryCanvas.cpp
)

target_include_directories(test_shaded_rasterizer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_shaded_rasterizer PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME shaded_rasterizer COMMAND test_shaded_rasterizer)

add_executable(test_renderer
    tests/test_renderer.cpp
    src/pipeline/Renderer.cpp
    src/pipeline/Rasterizer.cpp
    src/pipeline/ShadedRasterizer.cpp
    src/pipeline/Projection.cpp
    src/pipeline/Camera.cpp
    src/pipeline/DepthBuffer.cpp
    src/shading/Phong.cpp
    src/shading/Lighting.cpp
    src/math/Color.cpp
    src/math/Vec3.cpp
    src/mesh/Mesh.cpp
    src/mesh/Normals.cpp
    src/canvas/MemoryCanvas.cpp
)

target_include_directories(test_renderer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_renderer PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME renderer COMMAND test_renderer)
set_tests_properties(renderer PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(test_renderer_phong
    tests/test_renderer_phong.cpp
    src/pipeline/Renderer.cpp
    src/pipeline/Rasterizer.cpp
    src/pipeline/ShadedRasterizer.cpp
    src/pipeline/Projection.cpp
    src/pipeline/Camera.cpp
    src/pipeline/DepthBuffer.cpp
    src/shading/Phong.cpp
    src/shading/Lighting.cpp
    src/math/Color.cpp
    src/math/Vec3.cpp
    src/mesh/Mesh.cpp
    src/mesh/Normals.cpp
    src/canvas/MemoryCanvas.cpp
)

target_include_directories(test_renderer_phong PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_renderer_phong PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME renderer_phong COMMAND test_renderer_phong)
set_tests_properties(renderer_phong PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(test_model_coverage
    tests/test_model_coverage.cpp
    src/pipeline/Renderer.cpp
    src/pipeline/Rasterizer.cpp
    src/pipeline/ShadedRasterizer.cpp
    src/pipeline/Projection.cpp
    src/pipeline/Camera.cpp
    src/pipeline/DepthBuffer.cpp
    src/shading/Phong.cpp
    src/shading/Lighting.cpp
    src/math/Color.cpp
    src/math/Vec3.cpp
    src/mesh/Mesh.cpp
    src/mesh/Normals.cpp
    src/canvas/MemoryCanvas.cpp
)

target_include_directories(test_model_coverage PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_model_coverage PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME model_coverage COMMAND test_model_coverage)
set_tests_properties(model_coverage PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    TIMEOUT 30
)

add_executable(test_performance
    tests/test_performance.cpp
    src/pipeline/Renderer.cpp
    src/pipeline/Rasterizer.cpp
    src/pipeline/ShadedRasterizer.cpp
    src/pipeline/Projection.cpp
    src/pipeline/Camera.cpp
    src/pipeline/DepthBuffer.cpp
    src/shading/Phong.cpp
    src/shading/Lighting.cpp
    src/math/Color.cpp
    src/math/Vec3.cpp
    src/mesh/Mesh.cpp
    src/mesh/Normals.cpp
    src/canvas/MemoryCanvas.cpp
)

target_include_directories(test_performance PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_performance PRIVATE -Wall -Wextra -Wpedantic)
if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(test_performance PRIVATE -O3 -DNDEBUG)
endif()

add_test(NAME performance COMMAND test_performance)
set_tests_properties(performance PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    TIMEOUT 120
    RUN_SERIAL TRUE
)

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

add_test(NAME info_fora_do_diretorio
         COMMAND cpu-renderer --info ${CMAKE_SOURCE_DIR}/models/triangulo.byu)
set_tests_properties(info_fora_do_diretorio PROPERTIES
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    PASS_REGULAR_EXPRESSION "3 vértices, 1 triângulo"
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

add_test(NAME camera_padrao
         COMMAND ${CMAKE_COMMAND}
                 -DCPU_RENDERER=$<TARGET_FILE:cpu-renderer>
                 -DPROJECT_SOURCE_DIR=${CMAKE_SOURCE_DIR}
                 -DOUTPUT_DIR=${CMAKE_BINARY_DIR}
                 -P ${CMAKE_SOURCE_DIR}/tests/check_default_camera.cmake)
set_tests_properties(camera_padrao PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
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

add_test(NAME camera_policy
         COMMAND ${CMAKE_COMMAND}
                 -DPROJECT_SOURCE_DIR=${CMAKE_SOURCE_DIR}
                 -P ${CMAKE_SOURCE_DIR}/tests/check_camera_policy.cmake)
set_tests_properties(camera_policy PROPERTIES
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

add_test(NAME lighting_policy
         COMMAND ${CMAKE_COMMAND}
                 -DPROJECT_SOURCE_DIR=${CMAKE_SOURCE_DIR}
                 -P ${CMAKE_SOURCE_DIR}/tests/check_lighting_policy.cmake)
set_tests_properties(lighting_policy PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    TIMEOUT 10
)

add_test(NAME lighting_padrao
         COMMAND ${CMAKE_COMMAND}
                 -DCPU_RENDERER=$<TARGET_FILE:cpu-renderer>
                 -DPROJECT_SOURCE_DIR=${CMAKE_SOURCE_DIR}
                 -DOUTPUT_DIR=${CMAKE_BINARY_DIR}
                 -P ${CMAKE_SOURCE_DIR}/tests/check_default_lighting.cmake)
set_tests_properties(lighting_padrao PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    TIMEOUT 10
)

add_test(NAME lighting_inexistente
         COMMAND ${CMAKE_COMMAND}
                 -DCPU_RENDERER=$<TARGET_FILE:cpu-renderer>
                 -DPROJECT_SOURCE_DIR=${CMAKE_SOURCE_DIR}
                 -DOUTPUT_DIR=${CMAKE_BINARY_DIR}
                 -DCASO=ausente
                 -P ${CMAKE_SOURCE_DIR}/tests/check_lighting_error.cmake)
set_tests_properties(lighting_inexistente PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    TIMEOUT 10
)

add_test(NAME lighting_invalido
         COMMAND ${CMAKE_COMMAND}
                 -DCPU_RENDERER=$<TARGET_FILE:cpu-renderer>
                 -DPROJECT_SOURCE_DIR=${CMAKE_SOURCE_DIR}
                 -DOUTPUT_DIR=${CMAKE_BINARY_DIR}
                 -DCASO=invalido
                 -P ${CMAKE_SOURCE_DIR}/tests/check_lighting_error.cmake)
set_tests_properties(lighting_invalido PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    TIMEOUT 10
)

add_test(NAME smoke_iluminacao_invalida
         COMMAND ${CMAKE_COMMAND}
                 -DCPU_RENDERER=$<TARGET_FILE:cpu-renderer>
                 -DPROJECT_SOURCE_DIR=${CMAKE_SOURCE_DIR}
                 -DOUTPUT_DIR=${CMAKE_BINARY_DIR}
                 -DCASO=smoke_invalido
                 -P ${CMAKE_SOURCE_DIR}/tests/check_lighting_error.cmake)
set_tests_properties(smoke_iluminacao_invalida PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    TIMEOUT 10
)

add_test(NAME lighting_sucesso
         COMMAND ${CMAKE_COMMAND}
                 -DCPU_RENDERER=$<TARGET_FILE:cpu-renderer>
                 -DPROJECT_SOURCE_DIR=${CMAKE_SOURCE_DIR}
                 -DOUTPUT_DIR=${CMAKE_BINARY_DIR}
                 -P ${CMAKE_SOURCE_DIR}/tests/check_lighting_success.cmake)
set_tests_properties(lighting_sucesso PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    TIMEOUT 10
)

add_executable(test_color
    tests/test_color.cpp
    src/math/Color.cpp
)

target_include_directories(test_color PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_color PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME color COMMAND test_color)

add_executable(test_lighting
    tests/test_lighting.cpp
    src/shading/Lighting.cpp
    src/math/Color.cpp
    src/math/Vec3.cpp
)

target_include_directories(test_lighting PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_lighting PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME lighting COMMAND test_lighting)
set_tests_properties(lighting PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(test_normals
    tests/test_normals.cpp
    src/mesh/Normals.cpp
    src/mesh/Mesh.cpp
    src/math/Vec3.cpp
)

target_include_directories(test_normals PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_normals PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME normals COMMAND test_normals)
set_tests_properties(normals PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(test_phong
    tests/test_phong.cpp
    src/shading/Phong.cpp
    src/math/Color.cpp
    src/math/Vec3.cpp
)

target_include_directories(test_phong PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(test_phong PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME phong COMMAND test_phong)

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
    DEPENDS cpu-renderer test_mesh test_vec3 test_camera test_projection test_depth_buffer test_rasterizer test_shaded_rasterizer test_renderer test_renderer_phong test_model_coverage test_performance test_bmp_writer test_color test_lighting test_normals test_phong
    USES_TERMINAL
)
