add_library(GC_Core SHARED
    gc_server.cpp
    inventory/manager.cpp
    inventory/schema.cpp
    store/store.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/logging.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/crypto.cpp
    protobufs/cs2_gc.pb.cc
)

target_include_directories(GC_Core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
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