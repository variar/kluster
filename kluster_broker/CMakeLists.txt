file(GLOB SRCS *.cpp)

add_executable(kluster_broker ${SRCS})
target_link_libraries(kluster_broker transport)
