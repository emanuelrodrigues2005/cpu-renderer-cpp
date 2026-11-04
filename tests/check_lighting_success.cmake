if(NOT DEFINED CPU_RENDERER OR NOT DEFINED PROJECT_SOURCE_DIR OR NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "CPU_RENDERER, PROJECT_SOURCE_DIR e OUTPUT_DIR não foram informados")
endif()

set(dump "${OUTPUT_DIR}/dump-lighting-sucesso.bmp")
file(REMOVE "${dump}")

execute_process(
    COMMAND "${CPU_RENDERER}" --dump "${dump}"
            models/maca.byu camera/camera.txt lighting/lighting-pdf.txt
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    RESULT_VARIABLE resultado
    OUTPUT_VARIABLE saida
    ERROR_VARIABLE erro
)

if(NOT resultado EQUAL 0)
    message(FATAL_ERROR
            "o dump com o terceiro arquivo de iluminação falhou (código ${resultado})\n"
            "stdout:\n${saida}\n"
            "stderr:\n${erro}")
endif()

string(FIND "${saida}" "BMP salvo" mensagem_pos)
if(mensagem_pos EQUAL -1)
    message(FATAL_ERROR "stdout não contém 'BMP salvo':\n${saida}")
endif()

if(NOT EXISTS "${dump}")
    message(FATAL_ERROR "o BMP não foi criado: ${dump}")
endif()

file(SIZE "${dump}" tamanho)
if(tamanho LESS 55)
    message(FATAL_ERROR "o BMP foi criado sem conteúdo além do cabeçalho (${tamanho} bytes): ${dump}")
endif()
