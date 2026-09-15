if(NOT DEFINED PROJECT_SOURCE_DIR)
    message(FATAL_ERROR "PROJECT_SOURCE_DIR não foi informado")
endif()

file(READ "${PROJECT_SOURCE_DIR}/src/main.cpp" main_source)

if(NOT main_source MATCHES "camera/camera.txt")
    message(FATAL_ERROR "o caminho padrão de câmera deve ser camera/camera.txt")
endif()

file(GLOB_RECURSE fontes
     "${PROJECT_SOURCE_DIR}/src/*.cpp"
     "${PROJECT_SOURCE_DIR}/src/*.h")
foreach(fonte ${fontes})
    file(READ "${fonte}" conteudo)
    if(conteudo MATCHES "camera/presets")
        message(FATAL_ERROR "fonte ainda referencia presets de câmera: ${fonte}")
    endif()
endforeach()

if(EXISTS "${PROJECT_SOURCE_DIR}/camera/presets")
    message(FATAL_ERROR "camera/presets/ não deve existir: a câmera é única e compartilhada")
endif()
