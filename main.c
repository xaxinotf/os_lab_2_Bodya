#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PHILOSOPHER_COUNT 5

pthread_mutex_t forks[PHILOSOPHER_COUNT];

void* philosopher(void* num) {
    int philosopher_number = *(int*)num;
    int first_fork = philosopher_number;
    int second_fork = (philosopher_number + 1) % PHILOSOPHER_COUNT;

    // Змінюємо порядок для філософа, щоб уникнути deadlock, забезпечуючи, що кожен бере спочатку виделку з найменшим номером
    if (first_fork > second_fork) {
        int temp = first_fork;
        first_fork = second_fork;
        second_fork = temp;
    }

    while (1) {
        printf("Філософ %d думає.\n", philosopher_number);

        // Філософ намагається взяти першу виделку
        pthread_mutex_lock(&forks[first_fork]);
        printf("Філософ %d взяв виделку %d.\n", philosopher_number, first_fork);

        // Випадкова пауза перед спробою взяти другу виделку
        usleep(rand() % 100000);

        // Філософ намагається взяти другу виделку
        pthread_mutex_lock(&forks[second_fork]);
        printf("Філософ %d взяв виделку %d.\n", philosopher_number, second_fork);

        // Філософ їсть
        printf("Філософ %d їсть.\n", philosopher_number);
        usleep(rand() % 100000);

        // Філософ закінчив їсти і повертає виделки
        pthread_mutex_unlock(&forks[second_fork]);
        printf("Філософ %d поклав виделку %d.\n", philosopher_number, second_fork);
        pthread_mutex_unlock(&forks[first_fork]);
        printf("Філософ %d поклав виделку %d.\n", philosopher_number, first_fork);

        // Випадкова пауза перед наступним циклом думання-їжа
        usleep(rand() % 100000);
    }
    return NULL;
}

int main() {
    pthread_t philosophers[PHILOSOPHER_COUNT];
    int philosopher_numbers[PHILOSOPHER_COUNT];

    srand(time(NULL)); // Ініціалізація генератора випадкових чисел

    // Ініціалізація мьютексів для виделок
    for (int i = 0; i < PHILOSOPHER_COUNT; ++i) {
        pthread_mutex_init(&forks[i], NULL);
    }

    // Створення потоків для філософів
    for (int i = 0; i < PHILOSOPHER_COUNT; ++i) {
        philosopher_numbers[i] = i;
        if(pthread_create(&philosophers[i], NULL, philosopher, (void*)&philosopher_numbers[i])) {
            fprintf(stderr, "Помилка при створенні потоку філософа %d\n", i);
            return 1;
        }
    }

    // Очікування завершення роботи потоків філософів (хоча вони ніколи не завершуються в цьому безкінечному циклі)
    for (int i = 0; i < PHILOSOPHER_COUNT; ++i) {
        pthread_join(philosophers[i], NULL);
    }

    // Звільнення ресурсів мьютексів
    for (int i = 0; i < PHILOSOPHER_COUNT; ++i){
        pthread_mutex_destroy(&forks[i]);
    }

    return 0;
}
