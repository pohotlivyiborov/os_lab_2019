#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include "find_min_max.h"
#include "utils.h"

int timeout = 0; // Таймаут по умолчанию

void kill_children(pid_t *child_pids, int pnum) {
    for (int i = 0; i < pnum; i++) {
        if (child_pids[i] > 0) {
            kill(child_pids[i], SIGKILL);
        }
    }
}

void timeout_handler(int signum) {
    printf("Timeout reached, killing child processes...\n");
    exit(1);
}

int main(int argc, char **argv) {
    int seed = -1;
    int array_size = -1;
    int pnum = -1;
    bool with_files = false;

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {
            {"seed", required_argument, 0, 0},
            {"array_size", required_argument, 0, 0},
            {"pnum", required_argument, 0, 0},
            {"by_files", no_argument, 0, 'f'},
            {"timeout", required_argument, 0, 0},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0: // seed
                        seed = atoi(optarg);
                        if (seed <= 0) {
                            fprintf(stderr, "Error: seed must be a positive number.\n");
                            return 1;
                        }
                        break;
                    case 1: // array_size
                        array_size = atoi(optarg);
                        if (array_size <= 0) {
                            fprintf(stderr, "Error: array_size must be a positive number.\n");
                            return 1;
                        }
                        break;
                    case 2: // pnum
                        pnum = atoi(optarg);
                        if (pnum <= 0) {
                            fprintf(stderr, "Error: pnum must be a positive number.\n");
                            return 1;
                        }
                        break;
                    case 3: // by_files
                        with_files = true;
                        break;
                    case 4: // timeout
                        timeout = atoi(optarg);
                        if (timeout <= 0) {
                            fprintf(stderr, "Error: timeout must be a positive number.\n");
                            return 1;
                        }
                        break;
                    default:
                        printf("Index %d is out of options\n", option_index);
                }
                break;
            case 'f':
                with_files = true;
                break;
            case '?':
                break;
            default:
                printf("getopt returned character code 0%o?\n", c);
        }
    }

    if (optind < argc) {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (seed == -1 || array_size == -1 || pnum == -1) {
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n", argv[0]);
        return 1;
    }

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);
    int active_child_processes = 0;

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    int pipefd[2 * pnum]; // pipe array

    if (!with_files) {
        for (int i = 0; i < pnum; i++) {
            if (pipe(pipefd + i*2) < 0) {
                perror("pipe");
                return 1;
            }
        }
    }

    pid_t child_pids[pnum];

    for (int i = 0; i < pnum; i++) {
        pid_t child_pid = fork();
        if (child_pid >= 0) {
            active_child_processes += 1;
            if (child_pid == 0) {
                // Child process
                int chunk_size = array_size / pnum;
                int start_index = i * chunk_size;
                int end_index = (i == pnum - 1) ? array_size : start_index + chunk_size;

                struct MinMax min_max = GetMinMax(array, start_index, end_index);

                if (with_files) {
                    // Запись в файл
                    char filename[20];
                    sprintf(filename, "result_%d.txt", i);
                    FILE *file = fopen(filename, "w");
                    fprintf(file, "%d %d\n", min_max.min, min_max.max);
                    fclose(file);
                } else {
                    // Использование pipe
                    close(pipefd[i*2]); // закрыть канал для чтения
                    write(pipefd[i*2 + 1], &min_max, sizeof(struct MinMax));
                    close(pipefd[i*2 + 1]); // закрыть канал для записи
                }
                free(array);
                exit(0);
            } else {
                child_pids[i] = child_pid;
            }
        } else {
            printf("Fork failed!\n");
            return 1;
        }
    }

    if (timeout > 0) {
        signal(SIGALRM, timeout_handler);
        ualarm(timeout * 1000, 0);
    }

    while (active_child_processes > 0) {
        int status;
        pid_t pid = wait(&status);
        if (pid > 0) {
            active_child_processes -= 1;
        }
    }

    if (timeout > 0) {
        ualarm(0, 0); // Отключаем таймер, если все дочерние процессы завершились до истечения таймаута
    }

    struct MinMax min_max;
    min_max.min = INT_MAX;
    min_max.max = INT_MIN;

    for (int i = 0; i < pnum; i++) {
        if (with_files) {
            // Чтение (файл)
            char filename[20];
            sprintf(filename, "result_%d.txt", i);
            FILE *file = fopen(filename, "r");
            int min, max;
            fscanf(file, "%d %d", &min, &max);
            fclose(file);

            if (min < min_max.min) min_max.min = min;
            if (max > min_max.max) min_max.max = max;
        } else {
            // Чтение (pipes)
            struct MinMax child_min_max;
            close(pipefd[i*2 + 1]); // закрыть канал для записи
            read(pipefd[i*2], &child_min_max, sizeof(struct MinMax));
            close(pipefd[i*2]); // закрыть канал для чтения

            if (child_min_max.min < min_max.min) min_max.min = child_min_max.min;
            if (child_min_max.max > min_max.max) min_max.max = child_min_max.max;
        }
    }

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array);

    printf("Min: %d\n", min_max.min);
    printf("Max: %d\n", min_max.max);
    printf("Elapsed time: %fms\n", elapsed_time);
    fflush(NULL);
    return 0;
}