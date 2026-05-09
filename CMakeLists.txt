# =============================================================================
# FILE: CMakeLists.txt
# DESCRIPTION: CMake build configuration for Sign Language Translator.
#
# WHAT IS CMAKE?
#   CMake is a "meta build system" — it generates actual build files
#   (like Visual Studio projects or Makefiles) from this one CMakeLists.txt.
#   This makes your project portable across Windows, Linux, and Mac.
#
# HOW TO USE:
#   mkdir build && cd build
#   cmake .. -DCMAKE_BUILD_TYPE=Release
#   cmake --build . --config Release
# =============================================================================

# ─── Minimum CMake version ────────────────────────────────────────────────────
# We need CMake 3.18+ for modern C++ features and FindPackage improvements
cmake_minimum_required(VERSION 3.18)

# ─── Project definition ───────────────────────────────────────────────────────
# Sets project name, version, and language(s)
project(SignLanguageTranslator VERSION 1.0.0 LANGUAGES CXX)

# ─── C++ Standard ─────────────────────────────────────────────────────────────
# C++17 is required for:
#   - std::filesystem (create_directories, directory_iterator)
#   - Structured bindings (auto& [i, j] : CONNECTIONS)
#   - std::optional and other modern features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Error if C++17 not available
set(CMAKE_CXX_EXTENSIONS OFF)         # Use pure ISO C++ (no GNU extensions)

# ─── Build Type ───────────────────────────────────────────────────────────────
# Default to Release mode for better performance
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Optimization flags
set(CMAKE_CXX_FLAGS_RELEASE "/O2" CACHE STRING "" FORCE)      # MSVC optimization
set(CMAKE_CXX_FLAGS_DEBUG   "/Od /Zi" CACHE STRING "" FORCE)  # Debug with symbols

# ─── Windows-specific settings ────────────────────────────────────────────────
if(WIN32)
    # Disable warnings about deprecated Windows CRT functions
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    # Unicode support
    add_definitions(-DUNICODE -D_UNICODE)
    # Windows version targeting
    add_definitions(-D_WIN32_WINNT=0x0A00)  # Windows 10
endif()

# =============================================================================
# FIND OPENCV
# =============================================================================
# CMake will look for OpenCV using the FindOpenCV.cmake module.
# If not found automatically, set OpenCV_DIR:
#   cmake .. -DOpenCV_DIR="C:/opencv/build"
#
# Required OpenCV modules:
#   core     = Mat, basic operations
#   imgproc  = image processing (cvtColor, etc.)
#   highgui  = imshow(), waitKey(), namedWindow()
#   videoio  = VideoCapture (webcam access)
#   ml       = SVM and other ML classifiers
find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui videoio ml)

if(OpenCV_FOUND)
    message(STATUS "✓ OpenCV found: version ${OpenCV_VERSION}")
    message(STATUS "  OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
    message(STATUS "  OpenCV libraries: ${OpenCV_LIBS}")
else()
    message(FATAL_ERROR "✗ OpenCV NOT found! Install it and set OpenCV_DIR.")
endif()

# =============================================================================
# FIND MEDIAPIPE
# =============================================================================
# MediaPipe doesn't have standard CMake support.
# You need to build it manually or use a pre-built distribution.
#
# OPTION A: Set MEDIAPIPE_DIR to your built MediaPipe directory
# OPTION B: Use the mediapipe-cpp package (easier on Windows)
#
# Set these paths to match YOUR MediaPipe installation:
set(MEDIAPIPE_ROOT "" CACHE PATH "Path to MediaPipe root directory")
set(MEDIAPIPE_BUILD "" CACHE PATH "Path to MediaPipe build output directory")

# Check if MediaPipe paths are set
if("${MEDIAPIPE_ROOT}" STREQUAL "" OR "${MEDIAPIPE_BUILD}" STREQUAL "")
    message(WARNING
        "⚠ MEDIAPIPE_ROOT or MEDIAPIPE_BUILD not set!\n"
        "  Set them with:\n"
        "  cmake .. -DMEDIAPIPE_ROOT=C:/mediapipe -DMEDIAPIPE_BUILD=C:/mediapipe/build\n"
        "  \n"
        "  See SETUP_GUIDE.md for MediaPipe installation instructions.\n"
        "  Attempting to find MediaPipe automatically...\n"
    )
endif()

# Try to find MediaPipe include directory
find_path(MEDIAPIPE_INCLUDE_DIR
    NAMES "mediapipe/framework/calculator_framework.h"
    HINTS
        ${MEDIAPIPE_ROOT}
        "C:/mediapipe"
        "$ENV{USERPROFILE}/mediapipe"
    DOC "MediaPipe include directory"
)

# Try to find the main MediaPipe library
find_library(MEDIAPIPE_LIB
    NAMES
        mediapipe_framework
        mediapipe
        hand_tracking_cpu
    HINTS
        ${MEDIAPIPE_BUILD}
        ${MEDIAPIPE_BUILD}/Release
        ${MEDIAPIPE_BUILD}/Debug
    DOC "MediaPipe framework library"
)

# Check results
if(MEDIAPIPE_INCLUDE_DIR)
    message(STATUS "✓ MediaPipe headers found: ${MEDIAPIPE_INCLUDE_DIR}")
else()
    message(FATAL_ERROR
        "✗ MediaPipe headers NOT found!\n"
        "  Please set MEDIAPIPE_ROOT to your MediaPipe installation directory.\n"
        "  See the SETUP_GUIDE.md for detailed instructions.\n"
    )
endif()

# =============================================================================
# FIND PROTOBUF (required by MediaPipe)
# =============================================================================
# MediaPipe uses Protocol Buffers for its configuration format.
find_package(Protobuf REQUIRED)
if(Protobuf_FOUND)
    message(STATUS "✓ Protobuf found: ${Protobuf_VERSION}")
else()
    message(FATAL_ERROR "✗ Protobuf NOT found! Install it before building.")
endif()

# =============================================================================
# FIND ABSL (Abseil — Google's C++ library, used by MediaPipe)
# =============================================================================
find_package(absl REQUIRED)
if(absl_FOUND)
    message(STATUS "✓ Abseil found")
else()
    message(FATAL_ERROR "✗ Abseil NOT found! Install it via vcpkg or manually.")
endif()

# =============================================================================
# SOURCE FILES
# =============================================================================
# List all .cpp source files that make up our application.
set(SOURCES
    src/main.cpp
    src/hand_detector.cpp
    src/feature_extractor.cpp
    src/svm_trainer.cpp
    src/predictor.cpp
    src/utils.cpp
)

# =============================================================================
# CREATE EXECUTABLE
# =============================================================================
# add_executable() tells CMake: "compile these source files into an .exe"
add_executable(${PROJECT_NAME} ${SOURCES})

# =============================================================================
# INCLUDE DIRECTORIES
# =============================================================================
# These are the folders where the compiler looks for header files (#include "...").
target_include_directories(${PROJECT_NAME} PRIVATE
    # Our own headers
    ${CMAKE_SOURCE_DIR}/include

    # OpenCV headers
    ${OpenCV_INCLUDE_DIRS}

    # MediaPipe headers (root of mediapipe repo)
    ${MEDIAPIPE_INCLUDE_DIR}

    # Protobuf headers (usually found automatically)
    ${Protobuf_INCLUDE_DIRS}
)

# =============================================================================
# LINK LIBRARIES
# =============================================================================
# These are the .lib/.dll files the linker needs to create the final .exe
target_link_libraries(${PROJECT_NAME} PRIVATE
    # OpenCV modules
    ${OpenCV_LIBS}

    # MediaPipe
    ${MEDIAPIPE_LIB}

    # Protobuf
    ${Protobuf_LIBRARIES}

    # Abseil (many small libraries)
    absl::strings
    absl::str_format
    absl::status
    absl::statusor

    # Windows-specific libraries needed by OpenCV/MediaPipe
    $<$<PLATFORM_ID:Windows>:ws2_32>    # Windows sockets
    $<$<PLATFORM_ID:Windows>:winmm>     # Windows multimedia
)

# =============================================================================
# COMPILER WARNINGS
# =============================================================================
if(MSVC) # Microsoft Visual C++ compiler (used by VS Code + MSVC tools)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W3         # Warning level 3 (reasonable)
        /wd4244     # Suppress: conversion from double to float
        /wd4267     # Suppress: conversion from size_t to int
        /wd4996     # Suppress: deprecated CRT functions
        /MP         # Multi-processor compilation (faster builds)
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang") # GCC or Clang
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Wpedantic
        -Wno-deprecated-declarations
    )
endif()

# =============================================================================
# POST-BUILD: Copy DLLs to output directory (Windows only)
# =============================================================================
# On Windows, the .exe needs .dll files in the same folder to run.
# This command copies all necessary DLLs after building.
if(WIN32)
    # Copy OpenCV DLLs
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
    )

    # Copy OpenCV DLL manually (fallback)
    # Replace "opencv_world480.dll" with your OpenCV version
    file(GLOB OPENCV_DLLS "${OpenCV_DIR}/bin/*.dll")
    foreach(dll ${OPENCV_DLLS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${dll}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        )
    endforeach()
endif()

# =============================================================================
# INSTALL RULES (optional — for packaging the application)
# =============================================================================
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
install(DIRECTORY data/ DESTINATION data)
install(DIRECTORY models/ DESTINATION models OPTIONAL)
install(DIRECTORY assets/ DESTINATION assets OPTIONAL)

# =============================================================================
# PRINT BUILD SUMMARY
# =============================================================================
message(STATUS "")
message(STATUS "=== Build Configuration ===")
message(STATUS "Project:     ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type:  ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "OpenCV:      ${OpenCV_VERSION}")
message(STATUS "Compiler:    ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "===========================")
message(STATUS "")
