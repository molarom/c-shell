#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define STDIN_BUFFER 256

#define SHELL_PROMPT "#> "
#define SHELL_EXIT "exit"
#define SHELL_CLEAR "clear"

#define ANSI_RESET_CURSOR "\x1b[0;0H"
#define ANSI_CLEAR_SCREEN "\x1b[2J" 

#define ANSI_RED_TEXT "\x1b[31;40m"
#define ANSI_RESET_ATTR "\x1b[0m"

void handle_sigint(int signal)
{
	fprintf(stdout, "\n");
	fprintf(stdout, SHELL_PROMPT);	
	fflush(stdout);
}

void screen_clear()
{
	fprintf(stdout, ANSI_CLEAR_SCREEN);
	fflush(stdout);
	fprintf(stdout, ANSI_RESET_CURSOR);
}

void parse_input(char* input_buf) 
{
	char csh_exit[] = SHELL_EXIT;
	char csh_clear[] = SHELL_CLEAR;
	int exec_argc = 1;
	char** exec_argv = {0};
	char** exec_envp = {0};
	
	char* token;
	char* buf_cpy = strdup(input_buf);

	if ( strncmp( csh_exit, input_buf, 4 ) == 0 ) {
		exit(0);
	}

	exec_argv = malloc(sizeof(char *));

	if ( strchr(input_buf, ' ') == NULL ) {
		execve(input_buf, NULL, exec_envp);
	}

	// Get the number of items for the array.
	while ((token = strtok_r(buf_cpy, " ", &buf_cpy))) {
		exec_argv = realloc(exec_argv, (exec_argc + 1) *  sizeof(char *));
		exec_argv[exec_argc - 1] = strdup(token);	
		exec_argc++;
	}

	exec_argv[exec_argc] = NULL;

	// debugging
	for (int i = 0; i < exec_argc; i++)
		printf("exec_argv %d -> " ANSI_RED_TEXT  "%s\n" ANSI_RESET_ATTR, i, exec_argv[i]);
	
	execve(exec_argv[0], exec_argv, exec_envp);
	perror("execve");

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
