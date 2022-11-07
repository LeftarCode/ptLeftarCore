cmake_minimum_required (VERSION 3.8)

project ("ptLeftarCore")
add_subdirectory ("ptLeftarCore")

include(ExternalProject)
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/externals)

ExternalProject_Add(openexr
    GIT_REPOSITORY https://github.com/AcademySoftwareFoundation/openexr
    GIT_TAG v3.1.5
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
    BUILD_ALWAYS 1
)

add_dependencies(ptLeftarCore openexr)