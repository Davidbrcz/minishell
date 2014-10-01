FIND_PACKAGE(BISON REQUIRED)
FIND_PACKAGE(FLEX REQUIRED)

BISON_TARGET(LeafParser ${CMAKE_CURRENT_SOURCE_DIR}/gram.y ${CMAKE_CURRENT_BINARY_DIR}/gram.tab.c)
FLEX_TARGET(LeafScanner ${CMAKE_CURRENT_SOURCE_DIR}/gram.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(gram ${BISON_LeafParser_OUTPUTS} ${FLEX_LeafScanner_OUTPUTS})
add_library(stl mystring.c array_string.c) 
