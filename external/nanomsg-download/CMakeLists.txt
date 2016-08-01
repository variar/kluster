cmake_minimum_required(VERSION 2.8.2)

project(nanomsg-download NONE)

include(ExternalProject)
ExternalProject_Add(nanomsg-download
                    GIT_REPOSITORY;https://github.com/nanomsg/nanomsg.git;GIT_TAG;master;UPDATE_DISCONNECTED 1
                    SOURCE_DIR          "/home/omni/projects/Kasper/icfp/2016/tools/kluster/external/nanomsg-src"
                    BINARY_DIR          "/home/omni/projects/Kasper/icfp/2016/tools/kluster/external/nanomsg-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
)
