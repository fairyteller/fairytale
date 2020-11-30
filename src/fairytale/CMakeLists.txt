# add the executable
add_executable(fairytale main.cpp)

target_link_libraries(fairytale PUBLIC fairylib)

target_include_directories(fairytale PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/src/fairylib"
                          )