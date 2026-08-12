add_library(Injector SHARED
    dllmain.cpp
    hook.cpp
)

target_include_directories(Injector PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Injector
    GC_Core
)

if (WIN32)
    target_link_libraries(Injector
        funchook
    )
endif()
