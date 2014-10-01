include_directories (${PROJECT_SOURCE_DIR}/source/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)

add_subdirectory(lib)
add_executable(main main.c)	
target_link_libraries(main stl gram)
