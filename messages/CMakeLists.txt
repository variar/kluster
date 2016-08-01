file(GLOB SRCS *.cpp)

add_library(messages STATIC ${SRCS})
target_include_directories(messages PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(messages PUBLIC ${Boost_LIBRARIES})
