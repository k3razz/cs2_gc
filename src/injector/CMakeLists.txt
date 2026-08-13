add_library(Injector SHARED
    dllmain.cpp
    hook.cpp
)

target_include_directories(Injector PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Injector
    ${CMAKE_BINARY_DIR}/src/gc/Release/GC_Core.lib
)

if (WIN32)
    target_link_libraries(Injector
        ws2_32
    )
endif()

add_dependencies(Injector GC_Core)
