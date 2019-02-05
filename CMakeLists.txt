
cmake_minimum_required (VERSION 3.0)

project (GLEngine)
add_executable(GLEngine src/main.cpp)

add_definitions(-DGLEW_STATIC)

set_target_properties( GLEngine
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(GLEngine glfw GL GLEW)