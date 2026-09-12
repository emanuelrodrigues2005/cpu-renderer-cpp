if(NOT DEFINED PROJECT_SOURCE_DIR)
    message(FATAL_ERROR "PROJECT_SOURCE_DIR não foi informado")
endif()

file(READ "${PROJECT_SOURCE_DIR}/src/main.cpp" main_source)

if(main_source MATCHES "RenderMode mode = cg::RenderMode::Wireframe")
    message(FATAL_ERROR "o modo inicial deve ser Filled, não Wireframe")
endif()

if(NOT main_source MATCHES "RenderMode mode = cg::RenderMode::Filled")
    message(FATAL_ERROR "o modo inicial deve ser Filled")
endif()
