#include "lib.h"

int8_t toggle = FALSE;
int count = 3;
struct aiocb cb; // структура для асинхронных операций
char buffer[256];


void* thread_write(void* args) {
    int flags = O_CREAT | O_APPEND | O_WRONLY; // режим открытия файла (создать, в конец, запись)

    int cat_file = open("concatenated", flags, 0644);

    if(cat_file == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    while(count > 0) { // обрабатываем все 5 файлов
        while(toggle != TRUE) {
            usleep(100);
        }

        cb.aio_nbytes = strlen(buffer); // размер передачи
        cb.aio_fildes = cat_file; // файловый дескриптор

        // синхронная запись
        if(aio_write(&cb) == -1) {
            perror("aio_write");
            close(cat_file); // закрываем файл
            exit(EXIT_FAILURE);
        }

        while(EINPROGRESS == aio_error(&cb)){ // пока она продолжает выполнение
            // получаем статус завершения последней операции read/write
        }
        toggle = FALSE;
    }
    printf("Data has been written\n");
    pthread_exit(NULL);
}

void* thread_read(void *args) {
    char file_name[256];

    memset(&cb, 0, sizeof(cb));
    cb.aio_offset = 0; // смещение в файле для начала чтения/записи
    cb.aio_buf = buffer; // расположение буфера для чтения/записи

    for (int i = 1; i < 4; i++) {
        while(toggle == TRUE) {
            usleep(100);
        }

        sprintf(file_name, "%d", i); // записываем имя файла
        int current_file = open(file_name, O_RDONLY); // открываем файл в режиме чтения
        if(current_file == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        cb.aio_nbytes = 256; // размер передачи
        cb.aio_fildes = current_file; // файловый дескриптор

        if(aio_read(&cb) == -1) {
            perror("aio_read");
            close(current_file);
            exit(EXIT_FAILURE);
        }

        while(aio_error(&cb) == EINPROGRESS) {
            // получаем статус завершения последней операции read/write
        }

        count--;
        toggle = TRUE;

        printf("%s", buffer);

        close(current_file);
    }
    printf("Data has been read\n");

    pthread_exit(NULL);
}

