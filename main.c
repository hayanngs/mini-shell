#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/wait.h>
#include <pthread.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\e[0;32m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define TRUE 1
#define FALSE 0
#define COMMAND_LEN 100
#define PARAMS_LEN 2
#define PARAM_LEN 50

int read_command(char *command, char **parameters) {
    printf(ANSI_COLOR_RED "MS" ANSI_COLOR_RESET ":" ANSI_COLOR_GREEN "~%s$ " ANSI_COLOR_RESET, getcwd(NULL, 100));
    fgets(command, COMMAND_LEN, stdin);
    int i, cont_params = 0, index_param = 0;
    parameters[0] = malloc(PARAM_LEN);
    int troca = TRUE, back_ground = FALSE;
    for (i = 0; i < COMMAND_LEN && command[i] != '\n'; i++) {
        if (command[i] == '&') {
            back_ground = TRUE;
        } else if (command[i] == ' ' && troca) {
            parameters[cont_params][index_param] = '\0';
            index_param = 0;
            parameters[++cont_params] = malloc(PARAM_LEN);
            troca = FALSE;
        } else {
            parameters[cont_params][index_param++] = command[i];
        }
    }
    if (!index_param) {
        free(parameters[cont_params]);
        parameters[cont_params] = NULL;
    } else {
        parameters[cont_params][index_param] = '\0';
    }
    return back_ground;
}

void *clean_pointer(char **parameters) {
    for (int i = 0; i <= PARAMS_LEN; i++) {
        char *temp = parameters[i];
        parameters[i] = NULL;
        free(temp);
    }
}

int main() {
    char command[COMMAND_LEN];
    char **parameters = malloc(PARAMS_LEN);
    while (TRUE) {
        int back_ground = read_command(command, parameters);

        if (parameters[0][0] == 'c' && parameters[0][1] == 'd' && parameters[0][2] == '\0') {
            chdir(parameters[1]);
        } else if (parameters[0][0] == 'e' && parameters[0][1] == 'x' && parameters[0][2] == 'i' && parameters[0][3] == 't' && parameters[0][4] == '\0') {
            printf("> Finalizando o mini shell.\n");
            exit(0);
        } else {
            pid_t pid = fork();
            int status;

            switch (pid) {
                case -1:
                    perror("> Não foi possível criar um novo processo.");
                    exit(1);
                case 0:
                    execlp(parameters[0], parameters[0], parameters[1], 0);
                    break;
                default:
                    if (!back_ground) {
                        waitpid(pid, &status, 0);
                    }
                    break;
            }
            printf("> status: %d\n", status);
            if (WIFSIGNALED(status)) {
                printf("> Erro na execução do comando '%s'.\n", parameters[0]);
            } else if (WEXITSTATUS(status)) {
                printf("> Comando executado.\n");
            }
        }

        pthread_t a_thread;
        int res = pthread_create(&a_thread, NULL, (void *(*)(void *)) clean_pointer, parameters);
        if (res != 0) {
            perror("> Thread creation failed\n");
            exit(EXIT_FAILURE);
        }
    }
}
