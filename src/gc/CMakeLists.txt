set(PROTO_SRC ${CMAKE_CURRENT_BINARY_DIR}/cs2_gc.pb.cc)
set(PROTO_HDR ${CMAKE_CURRENT_BINARY_DIR}/cs2_gc.pb.h)

add_custom_command(
    OUTPUT ${PROTO_SRC} ${PROTO_HDR}
    COMMAND protobuf::protoc
    ARGS --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
         --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/protobufs
         ${CMAKE_CURRENT_SOURCE_DIR}/protobufs/cs2_gc.proto
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/protobufs/cs2_gc.proto
    COMMENT "Generating protobuf files"
)

add_library(GC_Core SHARED
    gc_server.cpp
    inventory/manager.cpp
    inventory/schema.cpp
    store/store.cpp
    ${PROTO_SRC}
)

target_include_directories(GC_Core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${_deps_protobuf-src}/src
)

target_link_libraries(GC_Core
    protobuf::libprotobuf
    nlohmann_json::nlohmann_json
    cryptopp
)

if (WIN32)
    target_link_libraries(GC_Core
        ws2_32
        crypt32
    )
endif()

if (UNIX)
    target_link_libraries(GC_Core
        pthread
    )
endif()
