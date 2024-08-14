cmake_minimum_required(VERSION 3.28)
project(course)

set(CMAKE_CXX_STANDARD 23)

file(GLOB common_src CONFIGURE_DEPENDS "common/*.h" "common/*.cpp")
file(GLOB server_src CONFIGURE_DEPENDS "server/*.h" "server/*.cpp")
file(GLOB client_src CONFIGURE_DEPENDS "client/*.h" "client/*.cpp")
add_executable(server
        ${common_src}
        ${server_src})
add_executable(client
        ${common_src}
        ${client_src})

set_target_properties( client server
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)


if(WIN32)
    target_link_libraries(server wsock32 ws2_32)
    target_link_libraries(client wsock32 ws2_32)
endif()
