cmake_minimum_required(VERSION 3.14) # FetchContent needs 3.11+
project(TerminalPortfolio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Fetch FTXUI from GitHub
include(FetchContent)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v5.0.0 # Use a specific stable version
)
FetchContent_MakeAvailable(ftxui)

# 2. Find libssh
find_package(libssh REQUIRED)

# 3. Add ALL your source files
add_executable(portfolio 
    src/main.cpp 
    src/server/server.cpp
    src/ui/main-window.cpp
    src/ui/tabs/about-me.cpp
    src/ui/tabs/contact-me.cpp
    src/ui/tabs/exp.cpp
)

# 4. Link everything together
target_link_libraries(portfolio 
    PRIVATE 
    ssh 
    ftxui::screen 
    ftxui::dom 
    ftxui::component
)