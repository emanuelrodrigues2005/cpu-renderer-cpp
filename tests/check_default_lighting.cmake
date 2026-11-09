if(NOT DEFINED CPU_RENDERER OR NOT DEFINED PROJECT_SOURCE_DIR OR NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "CPU_RENDERER, PROJECT_SOURCE_DIR e OUTPUT_DIR não foram informados")
endif()

set(dump_padrao "${OUTPUT_DIR}/dump-lighting-padrao.bmp")
set(dump_explicito "${OUTPUT_DIR}/dump-lighting-explicito.bmp")
set(dump_pdf "${OUTPUT_DIR}/dump-lighting-pdf.bmp")
file(REMOVE "${dump_padrao}" "${dump_explicito}" "${dump_pdf}")

execute_process(
    COMMAND "${CPU_RENDERER}" --dump "${dump_padrao}"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    RESULT_VARIABLE resultado_padrao
    OUTPUT_VARIABLE saida_padrao
    ERROR_VARIABLE erro_padrao
)
if(NOT resultado_padrao EQUAL 0)
    message(FATAL_ERROR
            "dump com o trio padrão completo (sem posicionais) falhou (código ${resultado_padrao})\n"
            "stdout:\n${saida_padrao}\n"
            "stderr:\n${erro_padrao}")
endif()

execute_process(
    COMMAND "${CPU_RENDERER}" --dump "${dump_explicito}"
            models/piramide.byu camera/camera.txt lighting/lighting.txt
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    RESULT_VARIABLE resultado_explicito
    OUTPUT_VARIABLE saida_explicito
    ERROR_VARIABLE erro_explicito
)
if(NOT resultado_explicito EQUAL 0)
    message(FATAL_ERROR
            "dump com o trio explícito (modelo, câmera e iluminação) falhou (código ${resultado_explicito})\n"
            "stdout:\n${saida_explicito}\n"
            "stderr:\n${erro_explicito}")
endif()

execute_process(
    COMMAND "${CPU_RENDERER}" --dump "${dump_pdf}"
            models/piramide.byu camera/camera.txt lighting/lighting-pdf.txt
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    RESULT_VARIABLE resultado_pdf
    OUTPUT_VARIABLE saida_pdf
    ERROR_VARIABLE erro_pdf
)
if(NOT resultado_pdf EQUAL 0)
    message(FATAL_ERROR
            "dump com lighting/lighting-pdf.txt falhou (código ${resultado_pdf})\n"
            "stdout:\n${saida_pdf}\n"
            "stderr:\n${erro_pdf}")
endif()

foreach(dump "${dump_padrao}" "${dump_explicito}" "${dump_pdf}")
    if(NOT EXISTS "${dump}")
        message(FATAL_ERROR "o dump esperado não foi criado: ${dump}")
    endif()
endforeach()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E compare_files "${dump_padrao}" "${dump_explicito}"
    RESULT_VARIABLE comparacao_padrao
    OUTPUT_QUIET
    ERROR_QUIET
)
if(NOT comparacao_padrao EQUAL 0)
    message(FATAL_ERROR "o dump padrão (sem posicionais) difere do dump com o trio explícito")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E compare_files "${dump_explicito}" "${dump_pdf}"
    RESULT_VARIABLE comparacao_pdf
    OUTPUT_QUIET
    ERROR_QUIET
)
if(comparacao_pdf EQUAL 0)
    message(FATAL_ERROR "lighting/lighting.txt e lighting/lighting-pdf.txt produziram o mesmo dump")
endif()
