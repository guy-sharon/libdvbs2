cmake_minimum_required(VERSION 3.20)

project(libdvbs2 VERSION 0.1.0 LANGUAGES C)

include(GNUInstallDirs)
include(CTest)

add_library(dvbs2 src/dvbs2.c)
add_library(dvbs2::dvbs2 ALIAS dvbs2)

target_compile_features(dvbs2 PUBLIC c_std_99)
target_include_directories(dvbs2
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(dvbs2 PROPERTIES
    C_EXTENSIONS OFF
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(TARGETS dvbs2
    EXPORT dvbs2Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT dvbs2Targets
    FILE dvbs2Targets.cmake
    NAMESPACE dvbs2::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dvbs2
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

option(DVBS2_BUILD_EXAMPLES "Build libdvbs2 examples" ON)
if(DVBS2_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
