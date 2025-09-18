cmake_minimum_required(VERSION 3.10)
project(CHTL CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectories for third-party libraries, utilities, the main library, and tests
add_subdirectory(ThirdParty)
add_subdirectory(Util)
add_subdirectory(CHTL)
add_subdirectory(Test)
