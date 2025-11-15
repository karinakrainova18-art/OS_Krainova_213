#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <ctype.h>

#define SHM_SIZE 1024

typedef struct {
    char data[SHM_SIZE];
    int data_size;
} shared_memory;

char to_lower(char c) {
    return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
}

void remove_extra_spaces(char* str, int* len) {
    int i = 0, j = 0;
    int prev_space = 0;
    
    for (i = 0; i < *len; i++) {
        if (str[i] == ' ') {
            if (!prev_space) {
                str[j++] = str[i];
            }
            prev_space = 1;
        } else {
            str[j++] = str[i];
            prev_space = 0;
        }
    }
    
    *len = j;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        const char* error_msg = "Usage: server <shm_name> <sem_empty> <sem_full>\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        exit(1);
    }

    const char* shm_name = argv[1];
    const char* sem_empty_name = argv[2];
    const char* sem_full_name = argv[3];

    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        const char* error_msg = "server: Ошибка создания shared memory\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        exit(1);
    }
    
    if (ftruncate(shm_fd, sizeof(shared_memory)) == -1) {
        const char* error_msg = "server: Ошибка установки размера shared memory\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        exit(1);
    }
    
    shared_memory* shm = mmap(0, sizeof(shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED) {
        const char* error_msg = "server: Ошибка mapping shared memory\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        exit(1);
    }

    shm->data_size = 0;

    sem_unlink(sem_empty_name);
    sem_unlink(sem_full_name);

    sem_t* sem_empty = sem_open(sem_empty_name, O_CREAT | O_EXCL, 0666, 1);
    if (sem_empty == SEM_FAILED) {
        const char* error_msg = "server: Ошибка создания семафора sem_empty\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        exit(1);
    }

    sem_t* sem_full = sem_open(sem_full_name, O_CREAT | O_EXCL, 0666, 0);
    if (sem_full == SEM_FAILED) {
        const char* error_msg = "server: Ошибка создания семафора sem_full\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        exit(1);
    }

    const char* ready_msg = "server: Готов к работе\n";
    write(STDOUT_FILENO, ready_msg, strlen(ready_msg));

    while (1) {
        sem_wait(sem_full);
        
        if (shm->data_size == -1) {
            break;
        }

        for (int i = 0; i < shm->data_size; i++) {
            shm->data[i] = to_lower(shm->data[i]);
        }
        
        remove_extra_spaces(shm->data, &shm->data_size);

        sem_post(sem_empty);
    }

    munmap(shm, sizeof(shared_memory));
    close(shm_fd);
    shm_unlink(shm_name);
    sem_close(sem_empty);
    sem_close(sem_full);
    sem_unlink(sem_empty_name);
    sem_unlink(sem_full_name);

    return 0;
}