#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_COMMAND_LENGTH 1024

void runCommand(char *command);

void createPipe(int *pipefd);

void closePipes(int pipes[][2], int n);

void waitChildProcesses(int *pids, int n);

// Global variable to track the alarm state
int alarm_seconds = 0;

// Signal handler for Ctrl+C (SIGINT)
void mysig_handler(int signo) {
    printf("\n ** This is the message from week09 lab2 - Signal Handler! ** \n");
}

// Signal handler for timer-alarm
void alarm_handler(int signo) {
    if (alarm_seconds > 0) {
        printf("\n ** Alarm is set for %d seconds! **\n", alarm_seconds);
        exit(0);
    }
}

void sig_handler(int signum){
    printf("** alarm interrupted after no activity, to be terminated **\n");
    kill(0, SIGTERM);
}

int main(int argc, char **argv) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = mysig_handler;
    sigaction(SIGINT, &sa, NULL);

    sa.sa_handler = alarm_handler;
    sigaction(SIGALRM, &sa, NULL);
    
    sa.sa_handler = sig_handler;
    sigaction(SIGTERM, &sa, NULL);

    char fullCommand[MAX_COMMAND_LENGTH];

    while (1) {
        printf("Shell>>: ");
        if (fgets(fullCommand, sizeof(fullCommand), stdin) == NULL) {
            perror("fgets");
            exit(1);
        }

        fullCommand[strcspn(fullCommand, "\n")] = '\0';

        if (strcmp(fullCommand, "exit") == 0) {
            exit(0);
        }

        if (strncmp(fullCommand, "alarm ", 6) == 0) {
            char *token = strtok(fullCommand, " ");
            token = strtok(NULL, " ");
            if (token) {
                int n = atoi(token);
                if (n > 0) {
                    alarm_seconds = n;
                    printf("Alarm is set for %d seconds!\n", alarm_seconds);
                    alarm(n);
                } else if (n == 0) {
                    alarm(0);
                    alarm_seconds = 0;
                    printf("Alarm is off!\n");
                } else {
                    printf("Invalid input for alarm command.\n");
                }
            } else {
                printf("Invalid input for alarm command.\n");
            }
        } else {
            int pipes[1][2];
            int pid, status;

            createPipe(pipes[0]);

            pid = fork();
            if (pid == 0) {
                // Child process
                dup2(pipes[0][1], 1);
                closePipes(pipes, 1);
                runCommand(fullCommand);
                exit(0);
            } else if (pid < 0) {
                perror("fork");
                exit(1);
            }

            pid = fork();
            if (pid == 0) {
                // Child process
                dup2(pipes[0][0], 0);
                closePipes(pipes, 1);
                runCommand(fullCommand);
                exit(0);
            } else if (pid < 0) {
                perror("fork");
                exit(1);
            }

            closePipes(pipes, 1);

            waitChildProcesses(NULL, 2);
        }
    }

    return 0;
}

void runCommand(char *command) {
    execl("/bin/sh", "sh", "-c", command, (char *)0);
    perror("execl");
    exit(1);
}

void createPipe(int *pipefd) {
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }
}

void closePipes(int pipes[][2], int n) {
    int i;
    for (i = 0; i < n; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

void waitChildProcesses(int *pids, int n) {
    int i;
    for (i = 0; i < n; i++) {
        int status;
        wait(&status);
        if (pids) {
            fprintf(stderr, "process %d exits with %d\n", pids[i], WEXITSTATUS(status));
        }
    }
}
