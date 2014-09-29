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
#include <sys/types.h>
#include <pwd.h>


const int FD_WRITTING = 1;
const int FD_READING = 0;

/**
   Allocate a string that will containt the string to display before the prompt 
   That string has to be deleted after. 
   @return the string to display before the prompt
*/
string format_pre_prompt() {
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;

    char* pwd = get_current_dir_name();
    string result;
    build_string(&result);
    char* r = strstr(pwd,homedir);
    if(r){       
        append_char(&result,'~');
	append_string(&result,r+strlen(homedir));
    }
    else{
        append_string(&result,pwd);
    }

    free(pwd);
    return result;
}

/**
   Will get the input from the user and return it as a string.
   
   Will exit the program if the user enters Ctrl+D
   @return a string containing all the input from the user
*/
string get_user_input() {
    string result = format_pre_prompt();
    printf("%s >",get_str(result));
    destruct_string(&result);

    if(fflush(stdout) != 0){
	exit(-1);
    }
    string s;
    build_string(&s); 
    
    char c;
    while( (c=fgetc(stdin)) !='\n')
    {
	if(EOF==c)
	{
	    printf("\n");
	    exit(0);
	}
	append_char(&s,c);
    }

    return s;
}

/**
  Will put into the array all the part of str separated by one 
  of the tokens in delim. The returned array has be destruct after use
  \return the parsed string in an array
*/
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

/**
   
*/
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
    }
    else if(0 == pid){
	//lunching itself with the user input on the CLI
	execlp(itself,itself,get_str(*s),NULL);
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

void execute_cmd(string full_cmd){
    array(string) input = tokenize(get_str(full_cmd)," ");

    size_t i = 0;
    while (i < size_array(string)(input)) {
	string tmp = get_elem_array(string)(input,i);
	char* tmpstr = get_str(tmp);
	if(*tmpstr == '<') {
	    dup(0);	    
	    close(0);
	    tmpstr++;
	    if (-1 == open(tmpstr,0)) {
		perror("open");
		exit(EXIT_FAILURE);
	    }
	    remove_elem_array(string)(&input,i);
	} else if(*tmpstr == '>'){
	    dup(1);
	    close(1);
	    tmpstr++;
	    if (-1 == open(tmpstr,1)) {
		perror("open");
		exit(EXIT_FAILURE);
	    }
	    remove_elem_array(string)(&input,i);
	} else ++i;
    }
    string cmd = get_elem_array(string)(input,0);

    array(char_ptr) args = build_excevp_args(input);
    execvp(get_str(cmd),get_array(char_ptr)(args));
}

void handle_recursion(char** argv)
{
    int pipe_fds[2];
    if(pipe(pipe_fds) == -1){
	perror("pipe");
	exit(EXIT_FAILURE);
    }
	    
    int pid = fork();
    array(string) input_cli = tokenize(argv[1],"|");
	    
    if(pid > 0) { 
	//father will execute the last command		
	close(pipe_fds[FD_WRITTING]);
	redirect_to(pipe_fds[FD_READING],STDIN_FILENO);
	close(pipe_fds[FD_READING]);

	//TODO Build a back acces function
	string last_command = get_elem_array(string)(input_cli,size_array(string)(input_cli)-1);

	execute_cmd(last_command);
    } else if (0 == pid){
	close(pipe_fds[FD_READING]);
	redirect_to(pipe_fds[FD_WRITTING],STDOUT_FILENO);
	close(pipe_fds[FD_WRITTING]);

	string leftover_cli;build_string(&leftover_cli);
	size_t  leftover_cli_nb = size_array(string)(input_cli)-1;

	for(size_t i = 0 ; i < leftover_cli_nb ; ++i) {
	    string tmp = get_elem_array(string)(input_cli,i);
	    append_string(&leftover_cli,get_str(tmp));
	    append_char(&leftover_cli,'|');
	}
	pop_string(&leftover_cli);

	execlp(argv[0],argv[0],get_str(leftover_cli),NULL);
    } else{
	perror("fork");
	exit(EXIT_FAILURE);
    }
}

int main(int argc,char* argv[])
{
    //there are no arguments on the CLI
    if(1 == argc) {
	while(1) {
	    string s = get_user_input();
	    handle_user_input(argv[0],&s);
	    destruct_string(&s);
	}
    } else if(argc > 1) {
	string tmp;build_string(&tmp);
	append_string(&tmp,argv[1]);

	unsigned int how_many_pipes = string_count_characters(tmp,'|');
	if(how_many_pipes == 0) {
	    execute_cmd(tmp);
	} else {
	    handle_recursion(argv);
	}
    } 

    return 0;
}
