include_directories (${PROJECT_SOURCE_DIR}/source/include)

add_subdirectory(lib)
add_executable(main main.c)	
target_link_libraries(main stl)
