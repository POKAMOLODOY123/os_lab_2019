
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

volatile sig_atomic_t timeout_flag = 0; // Флаг для таймаута

void handle_timeout(int sig) {
    timeout_flag = 1; // Устанавливаем флаг при получении сигнала
}

int main(int argc, char **argv) {
    int seed = -1;
    int array_size = -1;
    int pnum = -1;
    bool with_files = false;
    int timeout = -1;  // Добавляем переменную для таймаута

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {
            {"seed", required_argument, 0, 0},
            {"array_size", required_argument, 0, 0},
            {"pnum", required_argument, 0, 0},
            {"by_files", no_argument, 0, 'f'},
            {"timeout", required_argument, 0, 't'},  // Добавляем новый аргумент
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0:
                        seed = atoi(optarg);
                        break;
                    case 1:
                        array_size = atoi(optarg);
                        break;
                    case 2:
                        pnum = atoi(optarg);
                        break;
                    case 3:
                        with_files = true;
                        break;
                    case 4:
                        timeout = atoi(optarg);  // Установка таймаута
                        break;
                    default:
                        printf("Index %d is out of options\n", option_index);
                }
                break;
            case 'f':
                with_files = true;
                break;
            case 't':
                timeout = atoi(optarg);  // Установка таймаута
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
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--timeout \"num\"]\n", argv[0]);
        return 1;
    }

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);
    int active_child_processes = 0;

// Установка обработчика сигнала
    signal(SIGALRM, handle_timeout);

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    pid_t pids[pnum];  // Массив для хранения PID дочерних процессов

    for (int i = 0; i < pnum; i++) {
        pid_t child_pid = fork();
        if (child_pid >= 0) {
            // успешный fork
            active_child_processes += 1;
            pids[i] = child_pid;  // Сохраняем PID дочернего процесса
            if (child_pid == 0) {
                // дочерний процесс
                // здесь выполняем параллельные вычисления
                if (with_files) {
                    // использовать файлы
                } else {
                    // использовать pipe
                }
                return 0;
            }
        } else {
            printf("Fork failed!\n");
            return 1;
        }
    }

    // Проверка на таймаут
    if (timeout > 0) {
        alarm(timeout);  // Устанавливаем таймер таймаута
    }

    while (active_child_processes > 0) {
        if (timeout_flag) { // Если сработал таймаут
            for (int i = 0; i < pnum; i++) {
                kill(pids[i], SIGKILL);  // Убиваем дочерние процессы
            }
            printf("Timeout reached. Killed child processes.n");
            break;
        }

        int status;
        pid_t child_pid = wait(&status);
        if (child_pid > 0) {
            active_child_processes -= 1; // Уменьшение количества активно работающих процессов
        }
    }
    struct MinMax min_max;
    min_max.min = INT_MAX;
    min_max.max = INT_MIN;

    for (int i = 0; i < pnum; i++) {
        int min = INT_MAX;
        int max = INT_MIN;

        if (with_files) {
            // читать из файлов
        } else {
            // читать из пайпов
        }

        if (min < min_max.min) min_max.min = min;
        if (max > min_max.max) min_max.max = max;
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
