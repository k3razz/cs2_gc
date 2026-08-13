add_library(GC_Core SHARED
    gc_server.cpp
    inventory/manager.cpp
    inventory/schema.cpp
    store/store.cpp
    protobufs/base_gcmessages.pb.cc
    protobufs/econ_gcmessages.pb.cc
    protobufs/cstrike15_gcmessages.pb.cc
    protobufs/gcsdk_gcmessages.pb.cc
    protobufs/steammessages_gc.pb.cc
    protobufs/engine_gcmessages.pb.cc
)

target_include_directories(GC_Core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${_deps_protobuf-src}/src
)

target_link_libraries(GC_Core
    protobuf::libprotobuf
    nlohmann_json::nlohmann_json
    cryptopp
    ws2_32
    crypt32
)
