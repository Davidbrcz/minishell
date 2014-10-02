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
#include "bison.h"
#include "gram.tab.h"

const int FD_WRITTING = 1;
const int FD_READING = 0;

/**
   Allocate a string that will containt the string to display before the prompt 
   That string has to be deleted after. 
   \return the string to display before the prompt
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
   \return a string containing all the input from the user
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
  of the tokens in delim. The returned array has to be destruct after use
  \return the parsed string in an array
*/
array(string) tokenize(const char* str,const char* delim){
    size_t size = strlen(str);
    char* tmp=malloc((size+1)*sizeof(*tmp));
    strncpy(tmp,str,size);
    tmp[size]='\0';

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
   Converts the <array(string)> excevp args into an
   <array(char_ptr)>.
   \return the converted array
*/
array(char_ptr) build_excevp_args(array(string) arr){
    create_array(char_ptr, tmp);

    for(size_t i = 0; i < size_array(string)(arr) ; ++i){
	add_array(char_ptr)(&tmp,get_str(get_elem_array(string)(arr,i)));
    }

    add_array(char_ptr)(&tmp,NULL);
    return tmp;
}

/**
   Handles user input. Rejects inputs with faulty grammar.
   If user input begins by 'cd ', changes the working
   dir accordingly. Else, forks.
   ! WARNING ! cd handling only works under the condition 
   that ./main is launched using its full path.
   After forking, the father will wait its son while the son 
   launches itself with user input on the CLI.
*/
void handle_user_input(const char* itself ,string* s){
    YY_BUFFER_STATE buffer = yy_scan_string(get_str(*s));

    if(yyparse() == 0){
    }
    else{
        yy_delete_buffer(buffer);
	return ;
    }

    yy_delete_buffer(buffer);
    char* tmp = get_str(*s);

    if (tmp[0] == 'c' && tmp[1] == 'd' && tmp[2] == ' ') {
  	if (-1 == chdir(tmp+3)) {
	    perror("chdir");
	    exit(EXIT_FAILURE);
	}
    }
    else {
    	pid_t pid = fork();
    	if(pid > 0){
	   waitpid(pid,NULL,0);
    	}
    	else if(0 == pid){
	   execlp(itself,itself,tmp,NULL);
    	}
    	else{
	   perror("fork");
	   exit(EXIT_FAILURE);
   	}
   }
}

/**
   Redirects from pid <old> to pid <new>
*/
void redirect_to(int old,int new){
    if(dup2(old,new)==-1){
	perror("dup2");
	exit(EXIT_FAILURE);
    }
}

/**
   Redirects standard input to file <cmd>.
*/
void redirect_in(char* cmd) {
    int fd = open(cmd,O_RDONLY);

    if (-1 == fd) {
	perror("open");
	exit(EXIT_FAILURE);
    }

    dup2(fd,STDIN_FILENO);
    close(fd);
}

/**
   Redirects standard output to file <cmd>.
*/
void redirect_out(char* cmd, bool append) {
    int fd;
    int flag = O_WRONLY | O_CREAT ;
    if (append) {
    	flag |= O_APPEND;
    }
    fd = open(cmd,flag,S_IRWXU);
    if (-1 == fd) {
	perror("open");
	exit(EXIT_FAILURE);
    }

    dup2(fd,STDOUT_FILENO);
    close(fd);
}

/**
   Handles redirections.
   Only redirections written as <file >file or >>file will be handled.
   \return true if a redirection was handled.
*/
bool handle_redir_ifany(string tmp){
  char* tmpstr = get_str(tmp);

  if(*tmpstr == '<') {
    tmpstr++;
    redirect_in(tmpstr);
    return true;
  } else if(*tmpstr == '>'){
    tmpstr++;
    bool append = false;
    if (*tmpstr == '>') {
        tmpstr++;
	append = true;
    }
    redirect_out(tmpstr,append);
    return true;
  }

  return false;
}

/**
   Will merge quoted string that have been split in several strings and that should not have.
   \return true if a redirection was handled.
*/
void merge_split_string(array(string)* arr){
    array(string) new_array;
    build_array(string)(&new_array);

    for(size_t i = 0 ; i < size_array(string)(*arr) ; ++i){
	
	string str = get_elem_array(string)(*arr,i);	    
	if(get_str(str)[0]=='\"'){
	    string buf;
	    build_string(&buf);

	    string str2 = get_elem_array(string)(*arr,i);
	    char* c = get_str(str2);

	    while(c[size(str2)-1] != '\"'){
		append_string(&buf,c);
		append_char(&buf,' ');
		i++;

		str2 = get_elem_array(string)(*arr,i);
		c = get_str(str2);
	    }

	    pop_string(&str2);
	    append_string(&buf,c);

	    pop_front_string(&buf);
	    add_array(string)(&new_array,buf);
	}else{
	    add_array(string)(&new_array,str);	    
	}
    }
    delete_array(string)(arr);
    *arr=new_array;
}

/**
   Executes one command.
*/
void execute_cmd(string full_cmd){
    array(string) input = tokenize(get_str(full_cmd)," ");
    merge_split_string(&input);
    size_t i = 0;

    while (i < size_array(string)(input)) {

      string tmp = get_elem_array(string)(input,i);
      if(handle_redir_ifany(tmp)) {
	remove_elem_array(string)(&input,i);
      }
      else {
	++i;
      }
    } 

    string cmd = get_elem_array(string)(input,0);
    array(char_ptr) args = build_excevp_args(input);
    execvp(get_str(cmd),get_array(char_ptr)(args));
}

/**
   Handles the recursion. This function will be called with at
   least two pipes since the single command is handled in main.
   Creates the pipe between the last command and the rest of the
   input. After forking, the father will execute the last command
   while the son will call main with the updated args.
*/
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
	close(pipe_fds[FD_WRITTING]);
	redirect_to(pipe_fds[FD_READING],STDIN_FILENO);
	close(pipe_fds[FD_READING]);
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

/**
   Main.
*/
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
