#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "history.h"

#define STDIN_BUFFER 256
#define SHELL_PROMPT "#> "
#define SHELL_EXIT "exit"
#define SHELL_CLEAR "clear"
#define SHELL_HISTORY "history"

#define ANSI_RESET_CURSOR "\x1b[0;0H"
#define ANSI_CLEAR_SCREEN "\x1b[2J" 
#define ANSI_RED_TEXT "\x1b[31;40m"
#define ANSI_RESET_ATTR "\x1b[0m"

#define PPID 0

void handle_sigint(int signal)
{
	fprintf(stdout, "\n");
	fprintf(stdout, SHELL_PROMPT);	
	fflush(stdout);
}

void screen_clear()
{
	fprintf(stdout, ANSI_CLEAR_SCREEN);
	fprintf(stdout, ANSI_RESET_CURSOR);
	fflush(stdout);
}

void parse_input(char* input_buf) 
{
	char csh_exit[] = SHELL_EXIT;
	char csh_clear[] = SHELL_CLEAR;
    char csh_history[] = SHELL_HISTORY;
	int exec_argc = 1;
	char** exec_argv = {0};
	char** exec_envp = {0};
	pid_t pid = getpid();
	
	char* token;
	char* buf_cpy = strdup(input_buf);

	if ( strncmp( csh_exit, input_buf, 4 ) == 0 ) {
		exit(0);
	}

    // TODO: clean this up.
    // add free_list(list);
    List *list = create_list();

    add_command(list, input_buf);

    print_history(list);

	exec_argv = malloc(sizeof(char *));

	if ( strchr(input_buf, ' ') == NULL ) {
		execve(input_buf, NULL, exec_envp);
	}

	// Generate Null terminated string array.
	while ((token = strtok_r(buf_cpy, " ", &buf_cpy))) {
		token[strcspn(token,"\n")] = '\0';

		exec_argv = realloc(exec_argv, (exec_argc + 1) *  sizeof(char *));
		exec_argv[exec_argc - 1] = strdup(token);	
		exec_argc++;
	}

	exec_argv[exec_argc] = NULL;

	pid = fork();

	if ( pid == PPID ) {
		execve(exec_argv[0], exec_argv, exec_envp);
		perror("execve");
		_exit(0);
	} else if ( pid > (pid_t) PPID ) {
		wait(&pid);
	} else {
		fprintf(stderr, ANSI_RED_TEXT "Calling fork() failed." ANSI_RESET_ATTR);
		_exit(0);
	}

	// Free our malloc'd mem.	

	for (int i = 0; i < exec_argc; i++) {
		free(exec_argv[i]);
	}
	free(exec_argv);
		
	if ( strncmp( csh_clear, input_buf, 5 ) == 0 ) {
		screen_clear();
	}
}

int main(int argc, char** argv, char** envp)
{
	char line[STDIN_BUFFER] = {0};


    
	// Signal handling
	signal(SIGINT, handle_sigint);

	while (1 == 1) {
		fprintf(stdout, SHELL_PROMPT);
		fgets(line, sizeof(line), stdin);
				
		parse_input(line);
	}	
    
}
