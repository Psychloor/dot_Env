
cmake_minimum_required(VERSION 3.31)

project(dot_env
        VERSION 1.0.0
        DESCRIPTION "Modern C++ environment variables manager"
        HOMEPAGE_URL "https://github.com/yourusername/dot_env"
        LANGUAGES CXX
)

# Add option for system environment variable override
option(DOT_ENV_OVERRIDE_SYSTEM "Allow .env files to override system environment variables" OFF)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create library target
add_library(${PROJECT_NAME} STATIC
        src/env.cpp
        include/env.hpp
)

# Add an alias target for use in other projects
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Configure compile definitions based on options
if(DOT_ENV_OVERRIDE_SYSTEM)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DOT_ENV_OVERRIDE_SYSTEM)
endif()

# Set target properties
target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Generate and install config files
configure_package_config_file(
        cmake/dot_env-config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/dot_env-config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dot_env
)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/dot_env-config-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

# Installation
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}-targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp"
)