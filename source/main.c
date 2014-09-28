#define _GNU_SOURCE             
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

#include "mystring.h"
#include "array_string.h"

char delim_cmd[]={'|'};

const int FD_WRITTING = 1;
const int FD_READING = 0;

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
	//printf("---------Father waited %d--------\n",getpid());
    }
    else if(0 == pid){
	//printf("----------child %d-------------\n",getpid());
	//lunching itself with the user input on the CLI
	execlp(itself,itself,get_str(*s),NULL);
	//printf("error child \n");
    }
    else{
	perror("fork");
	exit(EXIT_FAILURE);
    }
}
void redirect_to(int old,int new){
    if(dup2(old,new)==-1){
	perror("dup2");
	exit(EXIT_FAILURE);
    }
}

void handle_recursion(char** argv)
{
    //printf("several commands \n");

    int pipe_fds[2];
    if(pipe(pipe_fds) == -1){
	perror("pipe");
	exit(EXIT_FAILURE);
    }
	    
    int pid = fork();
    array(string) input_cli = tokenize(argv[1],"|");
	    
    if(pid > 0) { 
	close(pipe_fds[FD_WRITTING]);
	//printf("father - several commands \n");

	//father will execute the last command		
	//TODO Build a back acces function
	string last_command = get_elem_array(string)(input_cli,size_array(string)(input_cli)-1);

	array(string) toks_current_cmd = tokenize(get_str(last_command)," ");
	string cmd = get_elem_array(string)(toks_current_cmd,0);
	array(char_ptr) args = build_excevp_args(toks_current_cmd);
	//printf("//////// \n");
	//printf("last cmd : %s \n", get_str(last_command));
	//printf("cmd : %s \n", get_str(cmd));
	for(size_t i = 0 ; i < args.size ; ++i){
	    //printf("args : %s \n", args.ptr[i]);
	}
	//printf("//////// \n");


	redirect_to(pipe_fds[FD_READING],STDIN_FILENO);
	close(pipe_fds[FD_READING]);

	execvp(get_str(cmd),get_array(char_ptr)(args));		    
    } else if (0 == pid){
	//printf("child,  several commands \n");
	close(pipe_fds[FD_READING]);

	string leftover_cli;build_string(&leftover_cli);
	size_t  leftover_cli_nb = size_array(string)(input_cli)-1;
	for(size_t i = 0 ; i < leftover_cli_nb ; ++i) {
	    string tmp = get_elem_array(string)(input_cli,i);
	    append_string(&leftover_cli,get_str(tmp));
	    append_char(&leftover_cli,'|');
	}
	pop_string(&leftover_cli);
	//printf("========= \n");
	//printf("bin : %s \n",argv[0]);
	print_string(leftover_cli);
	//printf("========= \n");	

	//printf("keep recursion \n");

	redirect_to(pipe_fds[FD_WRITTING],STDOUT_FILENO);
	close(pipe_fds[FD_WRITTING]);
	execlp(argv[0],argv[0],get_str(leftover_cli),NULL);
	//printf("error child, several cmds \n");
    } else{
	perror("fork");
	exit(EXIT_FAILURE);
    }
}

int main(int argc,char* argv[])
{
    //there are no arguments on the CLI
    if(1 == argc) {
	//printf("Init shell : %d \n",getpid());
	while(1) {
	    string s = get_user_input();
	    handle_user_input(argv[0],&s);
	    destruct_string(&s);
	}
    } else if(argc > 1) {
	////printf("executor : %d \n",getpid());

	string tmp;build_string(&tmp);
	append_string(&tmp,argv[1]);

	unsigned int how_many_pipes = string_count_characters(tmp,'|');
	//printf("nb of pipes = %d \n",how_many_pipes);

	if(how_many_pipes == 0) {
	    //printf("several commands, but only one left \n");
	    array(string) input = tokenize(get_str(tmp)," ");

	    string cmd = get_elem_array(string)(input,0);

	    array(char_ptr) args = build_excevp_args(input);
	    printf("cmd : %s \n", get_str(cmd));
	    for(size_t i = 0 ; i < args.size ; ++i){
		printf("args : %s \n", args.ptr[i]);
	    }
	    execvp(get_str(cmd),get_array(char_ptr)(args));
	} else {
	    handle_recursion(argv);
	}
    } else {
	//printf("CACA \n");
    }

    return 0;
}






