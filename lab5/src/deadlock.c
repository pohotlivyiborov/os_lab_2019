#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Два мьютекса для демонстрации deadlock
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

// Функция для первого потока
void* thread1(void* arg) {
    printf("Поток 1: Захватываю мьютекс 1...\n");
    pthread_mutex_lock(&mutex1);

    // Искусственная задержка для демонстрации
    sleep(1);

    printf("Поток 1: Пытаюсь захватить мьютекс 2...\n");
    pthread_mutex_lock(&mutex2);

    // Критическая секция (никогда не выполнится из-за deadlock)
    printf("Поток 1: Выполнение критической секции...\n");

    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);

    return NULL;
}

// Функция для второго потока
void* thread2(void* arg) {
    printf("Поток 2: Захватываю мьютекс 2...\n");
    pthread_mutex_lock(&mutex2);

    // Искусственная задержка для демонстрации
    sleep(1);

    printf("Поток 2: Пытаюсь захватить мьютекс 1...\n");
    pthread_mutex_lock(&mutex1);

    // Критическая секция (никогда не выполнится из-за deadlock)
    printf("Поток 2: Выполнение критической секции...\n");

    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);

    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Создание потоков
    pthread_create(&t1, NULL, thread1, NULL);
    pthread_create(&t2, NULL, thread2, NULL);

    // Ожидание завершения потоков
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Программа завершена.\n");

    return 0;
}