cmake_minimum_required(VERSION 3.20)
project(toy_rpc LANGUAGES CXX)

# --- C++ 20 Standard and Warnings ---
# Set the C++ 20 standard for ALL targets in this project
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Nice warnings (from your file)
if (MSVC)
  add_compile_options(/W4 /permissive- /Zc:__cplusplus)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor)
endif()


# ======================================================================
# 1. The RPC Runtime Library
# This is the core of your project (P1.2, P1.5)
# ======================================================================
add_library(toy_rpc_runtime
    src/calculator_runtime.cpp
    src/calculator_serde.cpp
    src/networking.cpp
    # TODO: Add your other files here, e.g.:
    # src/tcp_connection.cpp
    # src/serialization.cpp
)

# Make sure this library can find its own headers
target_include_directories(toy_rpc_runtime PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# If you use pthreads, asio, etc., you'll link them here
# Example: find_package(Threads REQUIRED)
# target_link_libraries(my_rpc_runtime PRIVATE Threads::Threads)


# ======================================================================
# 2. GoogleTest
# Fetch GTest and set it up
# ======================================================================
include(FetchContent)
FetchContent_Declare(
  GoogleTest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG    v1.17.0
)
FetchContent_MakeAvailable(GoogleTest)

# ======================================================================
# 3. The Failing Test Executable (P1.1)
# ======================================================================
add_executable(rpc_test
    test/rpc_test.cpp
)

# Link our test against:
# 1. Our RPC runtime (so it can use CalculatorClient/Server)
# 2. GTest (for the TEST_F and EXPECT_EQ macros)
target_link_libraries(rpc_test PRIVATE
    toy_rpc_runtime
    GTest::gtest_main # This links gtest and provides a main()
)


# ======================================================================
# 4. (Optional) Example Server/Client Executables
# These are useful for manual testing.
# ======================================================================

# --- Example Server User ---
add_executable(server
    src/server_main.cpp # You'll need to create this file
)
target_link_libraries(server PRIVATE toy_rpc_runtime)

# --- Example Client User ---
add_executable(client
    src/client_main.cpp # You'll need to create this file
)
target_link_libraries(client PRIVATE toy_rpc_runtime)
