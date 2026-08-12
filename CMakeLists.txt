cmake_minimum_required(VERSION 3.20)
project(CS2-Local-GC VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

find_package(Protobuf REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(detours REQUIRED)
find_package(OpenSSL REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gc/protobufs/cs2_gc.proto
)

add_library(GC_Core SHARED
    src/gc/gc_server.cpp
    src/gc/inventory/manager.cpp
    src/gc/inventory/schema.cpp
    src/gc/store/store.cpp
    src/utils/logging.cpp
    src/utils/crypto.cpp
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)

target_include_directories(GC_Core PRIVATE 
    include 
    src 
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(GC_Core 
    detours 
    nlohmann_json::nlohmann_json 
    protobuf::libprotobuf 
    OpenSSL::SSL
    OpenSSL::Crypto
    ws2_32
    crypt32
)

add_library(Injector SHARED 
    src/injector/dllmain.cpp 
    src/injector/hook.cpp
)

target_link_libraries(Injector 
    GC_Core 
    detours
)

if(DEFINED OUTDIR)
    install(TARGETS Injector GC_Core
        RUNTIME DESTINATION ${OUTDIR}
        LIBRARY DESTINATION ${OUTDIR}
    )
    
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.json 
         DESTINATION ${OUTDIR})
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/schema_2026.json 
         DESTINATION ${OUTDIR})
endif()
