if(NOT DEFINED PROJECT_SOURCE_DIR)
    message(FATAL_ERROR "PROJECT_SOURCE_DIR não foi informado")
endif()

file(READ "${PROJECT_SOURCE_DIR}/src/main.cpp" main_source)

if(NOT main_source MATCHES "lighting/lighting[.]txt")
    message(FATAL_ERROR "o caminho padrão de iluminação deve ser lighting/lighting.txt")
endif()

if(NOT main_source MATCHES "loadLightingFromFile")
    message(FATAL_ERROR "o main deve carregar a iluminação com loadLightingFromFile")
endif()

set(espaco "[ \t\r\n]*")
set(chamada_dump
    "renderMesh${espaco}\\(${espaco}mesh${espaco},${espaco}camera${espaco},${espaco}lighting${espaco},${espaco}canvas${espaco}\\)")
if(NOT main_source MATCHES "${chamada_dump}")
    message(FATAL_ERROR
            "o dump deve usar renderMesh(mesh, camera, lighting, canvas), com ou sem quebras de linha")
endif()

set(chamada_janela
    "renderMesh${espaco}\\(${espaco}mesh${espaco},${espaco}camera${espaco},${espaco}lighting${espaco},${espaco}window${espaco},${espaco}mode${espaco}\\)")
if(NOT main_source MATCHES "${chamada_janela}")
    message(FATAL_ERROR
            "a janela e o --smoke devem usar renderMesh(mesh, camera, lighting, window, mode), com ou sem quebras de linha")
endif()

string(FIND "${main_source}" "SDLK_r" reload_pos)
if(reload_pos EQUAL -1)
    message(FATAL_ERROR "o main deve tratar a tecla R (SDLK_r)")
endif()

string(SUBSTRING "${main_source}" ${reload_pos} -1 restante)
string(FIND "${restante}" "} else if" reload_end)
if(reload_end EQUAL -1)
    message(FATAL_ERROR "não foi possível delimitar o bloco de SDLK_r até o próximo '} else if'")
endif()
string(SUBSTRING "${restante}" 0 ${reload_end} reload_block)

foreach(simbolo
        "loadCamera(cameraPath, camera)"
        "loadLighting(lightingPath, lighting)"
        "cameraReloaded || lightingReloaded"
        "redraw()")
    string(FIND "${reload_block}" "${simbolo}" simbolo_pos)
    if(simbolo_pos EQUAL -1)
        message(FATAL_ERROR "o bloco de SDLK_r deve conter ${simbolo}")
    endif()
endforeach()

if(reload_block MATCHES "&&")
    message(FATAL_ERROR "o bloco de SDLK_r não deve usar && entre as recargas")
endif()

if(reload_block MATCHES "return")
    message(FATAL_ERROR "o bloco de SDLK_r não deve conter return")
endif()

if(reload_block MATCHES "exit")
    message(FATAL_ERROR "o bloco de SDLK_r não deve conter exit")
endif()
