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
#define SHELL_HIST_CLR "c_hist"
#define EXIT_NOERR 0

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

char** create_argv(char** exec_argv, char* input_str)
{
    int exec_argc = 1;
    char* token;

    while ((token = strtok_r(input_str, " ", &input_str))) {
        token[strcspn(token,"\n")] = '\0';

        exec_argv = realloc(exec_argv, (exec_argc + 1) *  sizeof(char *));
        exec_argv[exec_argc - 1] = strdup(token);
        exec_argc++;
    }

    exec_argv[exec_argc - 1] = NULL;

    return exec_argv;
}

// Free our malloc'd mem.
void free_argv(char** exec_argv)
{
    int i = 0;

    while (exec_argv[i] != NULL) {
        free(exec_argv[i]);
        i++;
    }

    free(exec_argv);
}

void execute_command(char* exec_file, char** exec_envp)
{
    pid_t pid = getpid();

    pid = fork();

    if ( pid == PPID ) {
        if (execve(exec_file, NULL, exec_envp) == -1)
            fprintf(stderr, ANSI_RED_TEXT "No such file or directory: " ANSI_RESET_ATTR "%s", exec_file);
        _exit(0);
    } else if ( pid > (pid_t) PPID ) {
        wait(&pid);
    } else {
        fprintf(stderr, ANSI_RED_TEXT "Calling fork() failed." ANSI_RESET_ATTR "\n");
        _exit(0);
    }
}

void execute_command_args(char** exec_argv, char** exec_envp)
{
    pid_t pid = getpid();

    pid = fork();

    if ( pid == PPID ) {
        if (execve(exec_argv[0], exec_argv, exec_envp) == -1)
            fprintf(stderr, ANSI_RED_TEXT "No such file or directory: " ANSI_RESET_ATTR "%s\n", exec_argv[0]);
        _exit(0);
    } else if ( pid > (pid_t) PPID ) {
        wait(&pid);
    } else {
        fprintf(stderr, ANSI_RED_TEXT "Calling fork() failed." ANSI_RESET_ATTR);
        _exit(0);
    }
}

int parse_command(char* input_buf, char** envp, List* history)
{
    char csh_exit[] = SHELL_EXIT;
    char csh_clear[] = SHELL_CLEAR;
    char csh_history[] = SHELL_HISTORY;
    char csh_hist_clear[] = SHELL_HIST_CLR;

    char** exec_argv = {0};

    if (strncmp(csh_exit, input_buf, 4) == 0 ) {
        exit(0);
        return EXIT_NOERR;
    }
    if (strncmp(csh_clear, input_buf, 5) == 0) {
        screen_clear();
        return EXIT_NOERR;
    }
    if (strncmp(csh_history, input_buf, 7) == 0) {
        print_history(history);
        return EXIT_NOERR;
    }
    if (strncmp(csh_hist_clear, input_buf, 6) == 0) {
        free_list(history);
        return EXIT_NOERR;
    }

    if ( strchr(input_buf, ' ') == NULL ) {
        execute_command(input_buf, envp);
        return EXIT_NOERR;
    }

    char* buf_cpy = strdup(input_buf);

    exec_argv = malloc(sizeof(char *));
    exec_argv = create_argv(exec_argv, buf_cpy);

    execute_command_args(exec_argv, envp);

    free_argv(exec_argv);

    return EXIT_NOERR;
}

int main(int argc, char** argv, char** envp)
{
        char line[STDIN_BUFFER] = {0};

    List *list = create_list();

    // Signal handling
    signal(SIGINT, handle_sigint);

    while (1 == 1) {
      fprintf(stdout, SHELL_PROMPT);
      fgets(line, sizeof(line), stdin);

      if (strcmp(line, ""))
        add_command(list, line);

      parse_command(line, envp, list);
    }

}
