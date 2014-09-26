#include "mystring.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void build_string(string* s) {
  s->size=0;
  s->capacity=MYSTRING_DEFAULT_CAPACITY;
  s->ptr=malloc(s->capacity*sizeof(*(s->ptr)));

  memset(s->ptr,'\0',s->capacity);
}

void destruct_string(string* s) {
  s->size=s->capacity=0;
  free(s->ptr);
}

int size(string s) {
  return s.size;
}

int capacity(string s) {
  return s.capacity;
}

char* get_str(string s) {
  return s.ptr;
}

void print_string(string s) {
  printf("%s@\n",s.ptr);
}

void append_string(string* s,const char* c) {
  int append_size=strlen(c);
  int i=0;
  if( (s->size+append_size) > (s->capacity-1))
    {
      if(!reserve(s,s->capacity+append_size+MYSTRING_DEFAULT_CAPACITY)){
   	printf("Out of mem, reserve failed in append_string");
	exit(-1);
      }
      memset(s->ptr+s->size,'\0',append_size+MYSTRING_DEFAULT_CAPACITY);   
   }


  for(i=0;i<append_size;++i)
    {
      s->ptr[s->size]=c[i]; 
      s->size++;
    }

}

int reserve(string* s,int new_capa) {  
  char* tmp = realloc (s->ptr,new_capa*sizeof(*tmp));

  if (tmp != NULL)
    {
      s->ptr = tmp;
      s->capacity=new_capa;

      return true;
    }
  else
    {
      /* l'ancien bloc est valide, mais il n'a pas ete agrandi */
      return false;
    }
}

void pop_string(string* s) {
  if(size(*s)!=0)
    {
      s->size--;
      s->ptr[s->size]='\0';
    }
}

void append_char(string* s,char c) {

  if(size(*s) +1 > capacity(*s))
    {
      if(!reserve(s,s->capacity+MYSTRING_DEFAULT_CAPACITY)) {
	  printf("Out of mem, reserve failed in append_char");
	  exit(-1);
	}
      memset(s->ptr+s->size,'\0',MYSTRING_DEFAULT_CAPACITY);
    }
  
  s->ptr[s->size]=c;
  s->size++;
}


void clear_string(string* s){
    memset(s->ptr, '\0', s->capacity);
    s->size = 0;
}
