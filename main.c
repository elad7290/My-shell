#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define NAME 0
#define MAX_SIZE 100
typedef struct {
    int pid;
    int numberOfArguments;
    char *arguments[MAX_SIZE];
} process;

int main() {
    int i, j;
    int flag_for_if = 0;
    process allProcess[100];
    int active_process = 0;
    char string[MAX_SIZE] = "";
    //shell
    printf("$ ");
    fflush(stdout);
    fgets(string, MAX_SIZE, stdin);
    string[strlen(string) - 1] = 0;
    char previous[100];
    getcwd(previous, MAX_SIZE);
    while (strcmp(string, "exit") != 0) {
        flag_for_if = 0;
        if (strcmp(string, "") == 0) {
            printf("An error occurred\n");
            //shell
            printf("$ ");
            fflush(stdout);
            fgets(string, MAX_SIZE, stdin);
            string[strlen(string) - 1] = 0;
            continue;
        }
        // add new process to array
        char *token = strtok(string, " ");
        allProcess[active_process].arguments[NAME] = (char *)malloc(sizeof(char) * sizeof(token));
        //check if malloc success
        if (allProcess[active_process].arguments[NAME] == NULL) {
            printf("An error occurred\n");
            //shell
            printf("$ ");
            fflush(stdout);
            fgets(string, MAX_SIZE, stdin);
            string[strlen(string) - 1] = 0;
            active_process++;
            continue;
        }
        strcpy(allProcess[active_process].arguments[NAME], token);
        i = 1;
        token = strtok(NULL, " ");
        while (token != NULL) {
            allProcess[active_process].arguments[i] = (char *)malloc(sizeof(char) * sizeof(token));
            if (allProcess[active_process].arguments[i] == NULL) {
                printf("An error occurred\n");
                //shell
                printf("$ ");
                fflush(stdout);
                fgets(string, MAX_SIZE, stdin);
                string[strlen(string) - 1] = 0;
                active_process++;
                continue;
            }
            strcpy(allProcess[active_process].arguments[i], token);
            i++;
            token = strtok(NULL, " ");
        }
        allProcess[active_process].numberOfArguments = i - 1;

        //jobs
        if (strcmp(allProcess[active_process].arguments[NAME], "jobs") == 0) {
            flag_for_if = 1;
            for (i = 0; i < active_process; ++i) {
                if ((kill(allProcess[i].pid, 0) == 0) && (allProcess[i].pid != 0)) {
                    //print all active non-build-in process
                    int size = allProcess[i].numberOfArguments + 1;
                    for (j = 0; j < size; ++j) {
                        if (strcmp(allProcess[i].arguments[j], "&")) {
                            printf("%s ", allProcess[i].arguments[j]);
                            fflush(stdout);
                        }
                    }
                    printf("\n");
                    fflush(stdout);
                }
            }
        }

        //history
        if (strcmp(allProcess[active_process].arguments[NAME], "history") == 0) {
            flag_for_if = 1;
            allProcess[active_process].pid = getpid();
            for (i = 0; i <= active_process; ++i) {
                //print argument AND print name
                int size = allProcess[i].numberOfArguments + 1;
                for (j = 0; j < size; ++j) {
                    if (strcmp(allProcess[i].arguments[j], "&")) {
                        printf("%s ", allProcess[i].arguments[j]);
                        fflush(stdout);
                    }
                }
                if ((kill(allProcess[i].pid, 0) == 0) && (allProcess[i].pid != 0)) {
                    printf("RUNNING");
                    fflush(stdout);
                } else {
                    printf("DONE");
                    fflush(stdout);
                }
                printf("\n");
                fflush(stdout);
            }
            allProcess[active_process].pid = 0;
        }

        //cd
        if (strcmp(allProcess[active_process].arguments[NAME], "cd") == 0) {
            flag_for_if = 1;
            int flag = 0;
            char *PATH = allProcess[active_process].arguments[1];
            char next[100];
            char *cd_token = strtok(PATH, "/");
            char temp[100];
            getcwd(temp, MAX_SIZE);
            if (allProcess[active_process].numberOfArguments == 0) {
                strcpy(next, getenv("HOME"));
                strcpy(previous, temp);
                if (chdir(next) != 0) {
                    printf("chdir failed\n");
                    fflush(stdout);
                    chdir(previous);
                    break;
                }
            } else if (allProcess[active_process].numberOfArguments == 1) {
                while (cd_token != NULL) {
                    if (strcmp("..", cd_token) == 0) {
                        strcpy(next, "..");
                    } else if (strcmp("~", cd_token) == 0) {
                        strcpy(next, getenv("HOME"));
                    } else if (strcmp("-", cd_token) == 0) {
                        strcpy(next, previous);
                    } else {
                        strcpy(next, cd_token);
                    }
                    cd_token = strtok(NULL, "/");
                    if (chdir(next) != 0) {
                        printf("chdir failed\n");
                        fflush(stdout);
                        chdir(temp);
                        flag = 1;
                        break;
                    }
                }
                if (flag == 0) {
                    strcpy(previous, temp);
                }
            } else {
                printf("Too many argument\n");
                fflush(stdout);
            }
        }

        //non-build-in process
        if (flag_for_if == 0) {
            int ret_code;
            int size = allProcess[active_process].numberOfArguments;
            if (strcmp(allProcess[active_process].arguments[0], "echo") == 0) {
                if (size == 0) {
                    printf("An error occurred\n");
                    //shell
                    printf("$ ");
                    fflush(stdout);
                    fgets(string, MAX_SIZE, stdin);
                    string[strlen(string) - 1] = 0;
                    active_process++;
                    continue;
                }
                int length = strlen(allProcess[active_process].arguments[1]) - 1;
                if ((allProcess[active_process].arguments[1][0] == 34) && (allProcess[active_process].arguments[1][length] == 34)) {
                    for (j = 1; j < length; ++j) {
                        allProcess[active_process].arguments[1][j - 1] = allProcess[active_process].arguments[1][j];
                    }
                    allProcess[active_process].arguments[1][length - 1] = 0;
                }
            }

            if (strcmp(allProcess[active_process].arguments[size], "&") != 0) {
                pid_t pid = fork();
                //check pid
                if (pid < 0) {
                    printf("fork failed\n");
                    fflush(stdout);
                }
                //parent
                if (pid > 0) {
                    allProcess[active_process].pid = pid;
                    waitpid(pid, NULL, 0);
                }
                //child
                if (pid == 0) {
                    ret_code = execvp(allProcess[active_process].arguments[NAME], allProcess[active_process].arguments);
                    if (ret_code == -1) {
                        printf("exec failed\n");
                        fflush(stdout);
                        kill(getpid(), SIGKILL);
                    }

                }

            } else {
                pid_t pid = fork();
                //check pid
                if (pid < 0) {
                    printf("fork failed\n");
                    fflush(stdout);
                }
                //parent
                if (pid > 0) {
                    allProcess[active_process].pid = pid;
                    signal(SIGCHLD, SIG_IGN);
                }
                //child
                if (pid == 0) {
                    int size = allProcess[active_process].numberOfArguments;
                    allProcess[active_process].arguments[size] = NULL;
                    ret_code = execvp(allProcess[active_process].arguments[NAME], allProcess[active_process].arguments);
                    if (ret_code == -1) {
                        printf("exec failed\n");
                        fflush(stdout);
                        kill(getpid(), SIGKILL);
                    }
                }
            }
        }


        //shell
        printf("$ ");
        fflush(stdout);
        fgets(string, MAX_SIZE, stdin);
        string[strlen(string) - 1] = 0;
        active_process++;
    }
    for (i = 0; i < active_process; ++i) {
        for (j = 0; j < allProcess[i].numberOfArguments; ++j) {
            free(allProcess[i].arguments[j]);
        }

    }
    return 0;
}


