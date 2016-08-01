if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(DownloadProject.cmake)

download_project(PROJ                nanomsg
                 PREFIX              ${CMAKE_CURRENT_BINARY_DIR}
                 GIT_REPOSITORY      https://github.com/nanomsg/nanomsg.git
                 GIT_TAG             master
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

set(NN_STATIC_LIB ON)
set(NN_ENABLE_DOC OFF)
set(NN_TESTS OFF)
set(NN_TOOLS OFF)

add_subdirectory(${nanomsg_SOURCE_DIR} ${nanomsg_BINARY_DIR})
target_include_directories(nanomsg PUBLIC ${nanomsg_SOURCE_DIR}/src)

download_project(PROJ                googletest
                 PREFIX              ${CMAKE_CURRENT_BINARY_DIR}
                 GIT_REPOSITORY      https://github.com/google/googletest.git
                 GIT_TAG             master
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
