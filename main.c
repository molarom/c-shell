#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define STDIN_BUFFER 256

#define SHELL_PROMPT "#> "
#define SHELL_EXIT "EXITS"
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
	
	// fprintf(stdout, "\ncmd to be run: %s\n", input_buf);

	char* token;
	char* buf_cpy = input_buf;

	while (( token = strtok_r(buf_cpy, " ", &buf_cpy)))
		fprintf(stdout, "\nToken -> " ANSI_RED_TEXT  "%s" "\n" ANSI_RESET_ATTR, token);

	if ( strncmp(input_buf, csh_exit, sizeof(*input_buf)) == 0 ) {
		exit(0);
	}
	
	if ( strncmp(input_buf, csh_clear, sizeof(*input_buf)) == 0 ) {
		screen_clear();
	}

}

int main(int argc, char** argv, char** envp)
{
	char line[STDIN_BUFFER] = {0};

	// Signal handling
	signal(SIGINT, handle_sigint);

	while(1 == 1) {
		fprintf(stdout, SHELL_PROMPT);
		fgets(line, sizeof(line), stdin);
				
		parse_input(line);	
	}	
}
