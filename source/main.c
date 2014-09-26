#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mystring.h"
#include "array_string.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

char delim_cmd[]={'|'};

string get_user_input() {
    printf("> ");

    if(fflush(stdout) != 0){
	exit(-1);
    }
    string s;
    build_string(&s); 
    
    char c;
    while( (c=fgetc(stdin)) !='\n')
    {
	append_char(&s,c);
    }
    return s;
}

array(string) tokenize(const char* str,const char* delim){
    size_t size = strlen(str);
    char* tmp=malloc((size+1)*sizeof(*tmp));
    strncpy(tmp,str,size+1);

    create_array(string,arr);
    char* tok = strtok(tmp,delim);
    while (tok) {
	string s;
	build_string(&s);
	append_string(&s,tok);
	add_array(string)(&arr,s);
	tok = strtok(NULL,delim);
    }
    return arr;
}
array(string) get_paths(){
    create_empty_array(string,tab);

    char* path = getenv ("PATH");
    if (path) {
	tab = tokenize(path,":");
    }

    for(size_t i=0;i<tab.size;++i)
    {
	print_string(tab.ptr[i]);
    }	        
    return tab;
}

array(char_ptr) build_excevp_args(array(string) arr){
    create_array(char_ptr, tmp);
    for(size_t i = 0; i < size_array(string)(arr) ; ++i){
	add_array(char_ptr)(&tmp,get_str(get_elem_array(string)(arr,i)));
    }
    add_array(char_ptr)(&tmp,NULL);
    return tmp;
}
void handle_user_input(const char* itself ,string* s){
    pid_t pid = fork();

    if(pid > 0){
	waitpid(pid,NULL,0);
	printf("---------Father %d--------\n",getpid());
    }
    else if(0 == pid){
	printf("----------child %d-------------\n",getpid());
	//lunching itself with the user input on the CLI
	execlp(itself,itself,get_str(*s),NULL);
	printf("error child \n");
    }
    else{
	printf("error while forking");
    }
}

int main(int argc,char* argv[])
{
    //there are no arguments on the CLI
    if(1 == argc)
    {
	printf("Init shell : %d \n",getpid());
	while(1) {
	    string s = get_user_input();
	    handle_user_input(argv[0],&s);
	    destruct_string(&s);
	}
    } else if(argc > 1){
	printf("executor : %d \n",getpid());
	string tmp;
	build_string(&tmp);
	append_string(&tmp,argv[1]);
	unsigned int how_many_pipes = string_count_characters(tmp,'|');
	if(how_many_pipes == 0){
	    array(string) input = tokenize(get_str(tmp)," ");

	    string cmd = get_elem_array(string)(input,0);
	    array(char_ptr) args = build_excevp_args(input);

	    execvp(get_str(cmd),get_array(char_ptr)(args));
	} else {
	    //handle_multiple_commands();
	}
    }

    return 0;
}
















