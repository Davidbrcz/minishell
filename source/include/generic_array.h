#ifndef MYARRAY_H 
#define MYARRAY_H 

#include <stdbool.h>

#define DEFAULT_SIZE_ARRAY 10

//template like macro
#define array(type) array_##type
#define build_array(type) build_array_##type
#define size_array(type) size_array_##type
#define get_array(type) get_array_##type
#define capacity_array(type) capacity_array_##type
#define reserve_array(type) reserve_array_##type
#define add_array(type) add_array_##type
#define get_elem_array(type) get_elem_array_##type
#define init_empty_array(type) init_empty_array_##type
#define remove_front_array(type) remove_front_array_##type
#define remove_elem_array(type) remove_elem_array_##type
#define delete_array(type) delete_array_##type

#define GENERIC_ARRAY_DEF(type)			\
    typedef struct array_##type array_##type;	\
    struct array_##type{			\
    type* ptr;					\
    size_t size;				\
    size_t capacity;                            \
    };						\
void build_array(type)(array(type)* );	\
void init_empty_array(type)(array(type)* );	\
size_t size_array(type)(array(type) );	\
size_t capacity_array(type)(array(type) );	\
void add_array(type)(array(type)*, type);	\
void remove_front_array(type)(array(type)*);	\
type* get_array(type)(array(type));            \
type get_elem_array(type)(array(type) ,size_t pos);	\
void remove_elem_array(type)(array(type)* ,size_t pos);	\
void delete_array(type)(array(type)* );	
#define create_array(type,name) \
    array(type) name;		\
    build_array(type)(&name);

#define create_empty_array(type,name) \
    array(type) name;		      \
    init_empty_array(type)(&name);

#define GENERIC_ARRAY_IMPL(type)				\
void build_array_##type(array(type)* tab){			\
    tab->ptr =	malloc(DEFAULT_SIZE_ARRAY*sizeof(*(tab->ptr)));	\
    tab->size = 0;						\
    tab->capacity = DEFAULT_SIZE_ARRAY;				\
}								\
void init_empty_array_##type(array(type)* tab){			\
    tab->ptr = 0;						\
    tab->size = 0;						\
    tab->capacity = 0;						\
}								\
size_t size_array_##type(array(type) tab) {			\
    return tab.size;						\
}								\
size_t capacity_array_##type(array(type) tab) {			\
    return tab.capacity;					\
}                                                               \
type* get_array_##type(array(type) tab) {			\
    return tab.ptr;						\
}								\
bool reserve_array_##type(array(type)* tab,size_t new_capa) {	\
    type* tmp = realloc (tab->ptr,new_capa*sizeof(*tmp));	\
    if (tmp != NULL)    {					\
	tab->ptr = tmp;						\
	if(new_capa < tab->capacity){				\
	    tab->size = new_capa;				\
	}							\
	tab->capacity=new_capa;					\
	return true;						\
    }								\
    else  {							\
	return false;						\
    }								\
}								\
void add_array_##type(array(type)* tab,type e){			\
    if(tab->size+1>tab->capacity){				\
	reserve_array(type)(tab,tab->size+DEFAULT_SIZE_ARRAY);	\
    }								\
    tab->ptr[tab->size] = e;					\
    tab->size++;						\
}								\
type get_elem_array_##type(array(type) tab,size_t pos){       	\
    if(pos > tab.size){						\
	exit(-1);						\
    }								\
    return tab.ptr[pos];					\
}								\
void remove_front_array_##type(array(type)* tab) {		\
  if(tab->size > 0) {						\
    remove_elem_array(type)(tab,0);				\
  }								\
}								\
 void remove_elem_array_##type(array(type)* tab,size_t pos) {   \
   for(size_t i = pos; i < tab->size-1 ;++i){			\
	tab->ptr[i]=tab->ptr[i+1];				\
   }								\
   tab->size--;							\
   tab->capacity++;						\
 }								\
 void delete_array_##type(array(type)* t){			\
     free(t->ptr);						\
     t->size=0;							\
     t->capacity=0;					\
}
#endif


















