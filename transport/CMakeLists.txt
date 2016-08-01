file(GLOB SRCS *.cpp)

add_library(transport STATIC ${SRCS})

target_include_directories(transport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(transport PUBLIC messages nanomsg zlibstatic)
