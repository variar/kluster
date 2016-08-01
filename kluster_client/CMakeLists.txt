file(GLOB SRCS *.cpp)

add_executable(kluster_client ${SRCS})

target_link_libraries(kluster_client transport)
