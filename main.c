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

#define FORK_SUCCESS 0

void handle_sigint(int signal)
{
    fprintf(stdout, "\n");
}

void screen_clear()
{
    fprintf(stdout, ANSI_CLEAR_SCREEN);
    fprintf(stdout, ANSI_RESET_CURSOR);
    fflush(stdout);
}

char** create_argv(char** exec_argv, char* input_str)
{
    int exec_argc = 0;
    char* token;

    while ((token = strtok_r(input_str, " ", &input_str))) {
        exec_argv = realloc(exec_argv, (exec_argc + 2) *  sizeof(char *));
        exec_argv[exec_argc] = strdup(token);
        exec_argc++;
    }
    exec_argv[exec_argc] = NULL;

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

void exec_cmd(char* exec_file)
{
    pid_t pid = getpid();
    pid = fork();

    // Verify if fork() was successfull.
    if ( pid < FORK_SUCCESS ) {
        fprintf(stderr, ANSI_RED_TEXT "Calling fork() failed." ANSI_RESET_ATTR "\n");
        _exit(0);
    } else if ( pid > (pid_t) FORK_SUCCESS ) {
        wait(&pid);
    } else {
        if (execve(exec_file, NULL, NULL) == -1) {
            fprintf(stderr, ANSI_RED_TEXT "No such file or directory:" ANSI_RESET_ATTR " %s\n", exec_file);
            _exit(0);
        }
    }
}

void exec_cmd_args(char** exec_argv, char** exec_envp)
{
    pid_t pid = getpid();
    pid = fork();

    if ( pid < FORK_SUCCESS ) {
        fprintf(stderr, ANSI_RED_TEXT "Calling fork() failed." ANSI_RESET_ATTR);
        _exit(0);
    } else if ( pid > (pid_t) FORK_SUCCESS ) {
        wait(&pid);
    } else {
        if (execve(exec_argv[0], exec_argv, exec_envp) == -1) {
            fprintf(stderr, ANSI_RED_TEXT "No such file or directory:" ANSI_RESET_ATTR " %s\n", exec_argv[0]);
            _exit(0);
        }
    }
}

static int shell_cmd(char* command, List *history)
{
    char csh_exit[] = SHELL_EXIT;
    char csh_clear[] = SHELL_CLEAR;
    char csh_history[] = SHELL_HISTORY;
    char csh_hist_clear[] = SHELL_HIST_CLR;

    if (strncmp(csh_exit, command, 4) == 0 ) {
        exit(0);
        return EXIT_NOERR;
    }
    if (strncmp(csh_clear, command, 5) == 0) {
        screen_clear();
        return EXIT_NOERR;
    }
    if (strncmp(csh_history, command, 7) == 0) {
        print_history(history);
        return EXIT_NOERR;
    }
    if (strncmp(csh_hist_clear, command, 6) == 0) {
        free_list(history);
        return EXIT_NOERR;
    }

    return EXIT_NOERR;
}

static int parse_cmd(char* input_buf, char** envp)
{
    input_buf[strcspn(input_buf,"\n")] = '\0';

    if (strchr(input_buf, ' ') == NULL) {
        exec_cmd(input_buf);
        return EXIT_NOERR;
    }

    char** exec_argv = {0};
    exec_argv = malloc(sizeof(char *));
    exec_argv = create_argv(exec_argv, input_buf);

    exec_cmd_args(exec_argv, envp);

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

      if (line[0] != '\0') {
        add_command(list, line);
        shell_cmd(line, list);
        parse_cmd(line, envp);
      }

    }

}
