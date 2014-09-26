#ifndef ARRAY_STRING_H 
#define ARRAY_STRING_H 

#include <stdlib.h>
#include <stdio.h>
#include "mystring.h"
#include "generic_array.h"

GENERIC_ARRAY_DEF(string);

typedef char* char_ptr;
GENERIC_ARRAY_DEF(char_ptr);

#endif
