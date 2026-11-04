if(NOT DEFINED CPU_RENDERER OR NOT DEFINED PROJECT_SOURCE_DIR OR NOT DEFINED OUTPUT_DIR OR NOT DEFINED CASO)
    message(FATAL_ERROR "CPU_RENDERER, PROJECT_SOURCE_DIR, OUTPUT_DIR e CASO não foram informados")
endif()

if(CASO STREQUAL "ausente")
    set(lighting_path "tests/fixtures/lighting-inexistente.txt")
    set(mensagem_esperada "não foi possível abrir")
    set(dump "${OUTPUT_DIR}/dump-lighting-erro-ausente.bmp")
elseif(CASO STREQUAL "invalido")
    set(lighting_path "tests/fixtures/lighting-range-invalido.txt")
    set(mensagem_esperada "Ka deve estar em [0,1]")
    set(dump "${OUTPUT_DIR}/dump-lighting-erro-invalido.bmp")
elseif(CASO STREQUAL "smoke_invalido")
    set(lighting_path "tests/fixtures/lighting-range-invalido.txt")
    set(mensagem_esperada "Ka deve estar em [0,1]")
else()
    message(FATAL_ERROR "CASO deve ser 'ausente', 'invalido' ou 'smoke_invalido', não '${CASO}'")
endif()

if(DEFINED dump)
    file(REMOVE "${dump}")
    execute_process(
        COMMAND "${CPU_RENDERER}" --dump "${dump}"
                models/triangulo.byu camera/camera.txt "${lighting_path}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        RESULT_VARIABLE resultado
        OUTPUT_VARIABLE saida
        ERROR_VARIABLE erro
    )
else()
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E env SDL_VIDEODRIVER=dummy
                "${CPU_RENDERER}" --smoke
                models/piramide.byu camera/camera.txt "${lighting_path}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        RESULT_VARIABLE resultado
        OUTPUT_VARIABLE saida
        ERROR_VARIABLE erro
    )
endif()

if(resultado EQUAL 0)
    message(FATAL_ERROR "o app deveria falhar com iluminação inválida (${lighting_path})")
endif()

if(NOT CASO STREQUAL "smoke_invalido")
    string(FIND "${erro}" "${lighting_path}" caminho_pos)
    if(caminho_pos EQUAL -1)
        message(FATAL_ERROR
                "stderr não menciona o caminho da iluminação ${lighting_path}\n"
                "stderr:\n${erro}")
    endif()
endif()

string(FIND "${erro}" "${mensagem_esperada}" mensagem_pos)
if(mensagem_pos EQUAL -1)
    message(FATAL_ERROR
            "stderr não contém a mensagem esperada '${mensagem_esperada}'\n"
            "stderr:\n${erro}")
endif()

string(FIND "${saida}" "BMP salvo" saida_bmp_pos)
if(NOT saida_bmp_pos EQUAL -1)
    message(FATAL_ERROR "stdout não deveria anunciar BMP salvo:\n${saida}")
endif()

if(DEFINED dump AND EXISTS "${dump}")
    message(FATAL_ERROR "o BMP não deveria ter sido criado: ${dump}")
endif()
