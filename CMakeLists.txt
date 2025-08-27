# ============================================
# CMakeLists.txt — AskLang Project
# ============================================

cmake_minimum_required(VERSION 3.16)
project(AskLang LANGUAGES CXX)

# --------------------------------------------
# Compiler settings
# --------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable more warnings
if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# --------------------------------------------
# Source files
# --------------------------------------------
file(GLOB_RECURSE ASKLANG_SOURCES
    src/*.cpp
)

# --------------------------------------------
# Include directories
# --------------------------------------------
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/runtime
)

# --------------------------------------------
# Build target
# --------------------------------------------
add_executable(ask ${ASKLANG_SOURCES})

# --------------------------------------------
# Install (optional)
# --------------------------------------------
install(TARGETS ask RUNTIME DESTINATION bin)

# --------------------------------------------
# Extra: show build type
# --------------------------------------------
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
