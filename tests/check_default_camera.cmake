if(NOT DEFINED CPU_RENDERER OR NOT DEFINED PROJECT_SOURCE_DIR OR NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "CPU_RENDERER, PROJECT_SOURCE_DIR e OUTPUT_DIR não foram informados")
endif()

set(dump_padrao "${OUTPUT_DIR}/dump-camera-padrao.bmp")
set(dump_explicito "${OUTPUT_DIR}/dump-camera-explicito.bmp")
file(REMOVE "${dump_padrao}" "${dump_explicito}")

execute_process(
    COMMAND "${CPU_RENDERER}" --dump "${dump_padrao}" models/piramide.byu
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    RESULT_VARIABLE resultado_padrao
    OUTPUT_QUIET
    ERROR_QUIET
)
if(NOT resultado_padrao EQUAL 0)
    message(FATAL_ERROR "dump com a câmera padrão falhou (código ${resultado_padrao})")
endif()

execute_process(
    COMMAND "${CPU_RENDERER}" --dump "${dump_explicito}"
            models/piramide.byu camera/camera.txt
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    RESULT_VARIABLE resultado_explicito
    OUTPUT_QUIET
    ERROR_QUIET
)
if(NOT resultado_explicito EQUAL 0)
    message(FATAL_ERROR "dump com camera/camera.txt falhou (código ${resultado_explicito})")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E compare_files "${dump_padrao}" "${dump_explicito}"
    RESULT_VARIABLE comparacao
)
if(NOT comparacao EQUAL 0)
    message(FATAL_ERROR "a câmera padrão do app não é camera/camera.txt")
endif()
