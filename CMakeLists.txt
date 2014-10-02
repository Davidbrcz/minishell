cmake_minimum_required (VERSION 2.8)
project (MINISHELL)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra -std=c99")  
add_subdirectory (source)
