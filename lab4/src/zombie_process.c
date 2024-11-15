#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t child_pid;

    // Создаем дочерний процесс
    child_pid = fork();

    if (child_pid > 0) {
        // Это родительский процесс
        printf("Родительский процесс: PID = %d\n", getpid());
        printf("Родительский процесс: Дочерний PID = %d\n", child_pid);

        // Родительский процесс не вызывает wait(), чтобы дождаться завершения дочернего процесса
        sleep(100); // Подождем 10 секунд, чтобы увидеть зомби-процесс

        printf("Родительский процесс: Завершаем работу\n");
    } else if (child_pid == 0) {
        // Это дочерний процесс
        printf("Дочерний процесс: PID = %d\n", getpid());
        printf("Дочерний процесс: Завершаем работу\n");
        exit(0); // Дочерний процесс завершается
    } else {
        // Ошибка при создании дочернего процесса
        perror("fork");
        exit(1);
    }

    return 0;
}