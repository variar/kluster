set(NN_STATIC_LIB ON)
set(NN_ENABLE_DOC OFF)
set(NN_TESTS OFF)
set(NN_TOOLS OFF)

add_subdirectory(nanomsg-master)
target_include_directories(nanomsg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/nanomsg-master/src)

add_subdirectory(tiny-process-library-master)
target_include_directories(tiny-process-library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tiny-process-library-master)
