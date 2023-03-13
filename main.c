#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/wait.h>

#define TRUE 1
#define COMMAND_LEN 100
#define PARAMS_LEN 20
#define PARAM_LEN 40

int read_command(char *command, char **parameters) {
    printf("MS:~%s$ ", getcwd(NULL, 100));
    fgets(command, COMMAND_LEN, stdin);
    int i, cont_params = 0, index_param = 0;
    parameters[0] = malloc(PARAM_LEN);
    for (i = 0; i < COMMAND_LEN && command[i] != '\n'; i++) {
        if (command[i] == ' ') {
            parameters[cont_params][index_param] = '\0';
            index_param = 0;
            cont_params++;
            parameters[cont_params] = malloc(PARAM_LEN);
        } else {
            parameters[cont_params][index_param++] = command[i];
        }
    }
    parameters[cont_params][index_param] = '\0';
    return cont_params;
}

int main() {
    char command[COMMAND_LEN];
    char **parameters = malloc(PARAMS_LEN);
    while (TRUE) {
        int num_params = read_command(command, parameters);

        if (parameters[0][0] == 'c' && parameters[0][1] == 'd' && parameters[0][2] == '\0') {
            chdir(parameters[1]);
        } else if (parameters[0][0] == 'e' && parameters[0][1] == 'x' && parameters[0][2] == 'i' && parameters[0][3] == 't' && parameters[0][4] == '\0') {
            printf("> Finalizando o mini shell.");
            exit(0);
        } else {
            pid_t pid = fork();
            int status;

            switch (pid) {
                case -1:
                    perror("> Não foi possível criar um novo processo.");
                    exit(1);
                case 0:
                    printf("> Executando o comando '%s'.\n", parameters[0]);
                    execlp(parameters[0], parameters[0], parameters[1], 0);
                    break;
                default:
                    waitpid(pid, &status, 0);
                    break;
            }
            printf("> status: %d\n", status);
            if (WIFSIGNALED(status)) {
                printf("> Erro na execução do comando '%s'.\n", parameters[0]);
            } else if (WEXITSTATUS(status)) {
                printf("> Comando executado.\n");
            }
        }

        for (int i = 0; i <= num_params; i++) {
            char *temp = parameters[i];
            parameters[i] = NULL;
            free(temp);
        }
    }
}
