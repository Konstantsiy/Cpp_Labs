#include <pthread.h>
#include "lib.h"

int main()
{
    pthread_t writerThread, readerThread; // создаём потоки для чтения и записи
    // создаём потоки
    pthread_create(&writerThread, NULL, &thread_write, NULL);
    pthread_create(&readerThread, NULL, &thread_read, NULL);
    // ожидаем завершения потоков
    pthread_join(writerThread, NULL);
    pthread_join(readerThread, NULL);
    return 0;
}

