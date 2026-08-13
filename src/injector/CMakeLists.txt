add_library(Injector SHARED
    dllmain.cpp
    hook.cpp
)

target_include_directories(Injector PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

find_library(GC_CORE_LIB GC_Core)
if(GC_CORE_LIB)
    target_link_libraries(Injector ${GC_CORE_LIB} ws2_32)
else()
    target_link_libraries(Injector GC_Core ws2_32)
endif()

add_dependencies(Injector GC_Core)
