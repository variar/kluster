cmake_minimum_required(VERSION 2.8.2)

project(tpl-download NONE)

include(ExternalProject)
ExternalProject_Add(tpl-download
                    GIT_REPOSITORY;https://github.com/variar/tiny-process-library.git;GIT_TAG;master;UPDATE_DISCONNECTED 1
                    SOURCE_DIR          "/home/omni/projects/Kasper/icfp/2016/tools/kluster/external/tpl-src"
                    BINARY_DIR          "/home/omni/projects/Kasper/icfp/2016/tools/kluster/external/tpl-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
)
