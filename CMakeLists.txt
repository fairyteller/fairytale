cmake_minimum_required (VERSION 2.8.11)
project (fairytale)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory (src/fairytale)
add_subdirectory (src/fairylib)
#add_subdirectory (src/tests)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT fairytale) 