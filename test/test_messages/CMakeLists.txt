file(GLOB SRCS *.cpp)

add_executable(test_messages ${SRCS})

target_link_libraries(test_messages
    messages
    gtest
    ${GTEST_LIBRARIES}
    ${GMOCK_LIBRARIES}
)
