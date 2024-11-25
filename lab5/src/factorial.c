#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
// Глобальные переменные
int k, pnum, mod;
long long result = 1;
pthread_mutex_t mutex;
// Функция, которую будет выполнять каждый поток
void* factorial_thread(void* arg) {
    int thread_id = *(int*)arg;
    long long local_result = 1;
    for (int i = thread_id + 1; i <= k; i += pnum) {
        local_result = (local_result * i) % mod;
    }
    // Блокировка мьютекса перед обновлением общего результата
    pthread_mutex_lock(&mutex);
    result = (result * local_result) % mod;
    pthread_mutex_unlock(&mutex);
    return NULL;
}
int main(int argc, char* argv[]) {
    int opt;
    // Инициализация переменных
    k = -1;
    pnum = -1;
    mod = -1;
    // Парсинг аргументов командной строки с использованием getopt
    while ((opt = getopt(argc, argv, "k:p:m:")) != -1) {
        switch (opt) {
            case 'k':
                k = atoi(optarg);
                break;
            case 'p':
                pnum = atoi(optarg);
                break;
            case 'm':
                mod = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Использование: %s -k <число> -p <потоки> -m <модуль>\n", argv[0]);
                return 1;
        }
    }
    if (k <= 0 || pnum <= 0 || mod <= 0) {
        fprintf(stderr, "Использование: %s -k <число> -p <потоки> -m <модуль>\n", argv[0]);
        return 1;
    }
    // Инициализация мьютекса
    pthread_mutex_init(&mutex, NULL);
    // Создание массива потоков
    pthread_t threads[pnum];
    int thread_ids[pnum];
    for (int i = 0; i < pnum; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, factorial_thread, &thread_ids[i]);
    }
    // Ожидание завершения всех потоков
    for (int i = 0; i < pnum; i++) {
        pthread_join(threads[i], NULL);
    }
    // Уничтожение мьютекса
    pthread_mutex_destroy(&mutex);
    // Вывод результата
    printf("Факториал %d по модулю %d: %lld\n", k, mod, result);
    return 0;
}