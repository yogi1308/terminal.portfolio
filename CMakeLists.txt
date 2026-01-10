cmake_minimum_required(VERSION 3.10)
project(TerminalPortfolio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Locate the libssh library
find_package(libssh REQUIRED)

# 2. Define the executable and its source files
add_executable(portfolio 
    src/main.cpp 
    src/server/server.cpp
)

# 3. Connect the library code to your program
target_link_libraries(portfolio PRIVATE ssh)