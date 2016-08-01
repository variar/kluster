file(GLOB SRCS *.cpp)

add_executable(kluster_worker ${SRCS})

target_link_libraries(kluster_worker transport tiny-process-library)
