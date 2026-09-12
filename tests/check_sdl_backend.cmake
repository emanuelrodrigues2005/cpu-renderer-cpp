if(NOT DEFINED PROJECT_SOURCE_DIR)
    message(FATAL_ERROR "PROJECT_SOURCE_DIR não foi informado")
endif()

file(READ "${PROJECT_SOURCE_DIR}/src/canvas/WindowSDL.cpp" window_source)

foreach(forbidden_call
        "SDL_RenderClear"
        "SDL_RenderCopy"
        "SDL_RenderDrawLine"
        "SDL_RENDERER_ACCELERATED")
    if(window_source MATCHES "${forbidden_call}")
        message(FATAL_ERROR "chamada SDL proibida no backend: ${forbidden_call}")
    endif()
endforeach()

if(NOT window_source MATCHES "SDL_RenderDrawPoint")
    message(FATAL_ERROR "o backend deve desenhar usando SDL_RenderDrawPoint")
endif()

if(NOT window_source MATCHES "SDL_RENDERER_SOFTWARE")
    message(FATAL_ERROR "o backend deve usar SDL_RENDERER_SOFTWARE")
endif()
