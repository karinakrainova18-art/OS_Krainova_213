#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>

#define SHM_SIZE 1024

typedef struct {
    char data[SHM_SIZE];
    int data_size;
} shared_memory;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        const char* error_msg = "Usage: client <shm_name> <sem_empty> <sem_full>\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        exit(1);
    }

    const char* shm_name = argv[1];
    const char* sem_empty_name = argv[2];
    const char* sem_full_name = argv[3];

    int attempts = 0;
    sem_t* sem_empty = SEM_FAILED;
    sem_t* sem_full = SEM_FAILED;
    
    while (attempts < 10) {
        sem_empty = sem_open(sem_empty_name, 0);
        sem_full = sem_open(sem_full_name, 0);
        
        if (sem_empty != SEM_FAILED && sem_full != SEM_FAILED) {
            break;
        }
        
        if (sem_empty != SEM_FAILED) sem_close(sem_empty);
        if (sem_full != SEM_FAILED) sem_close(sem_full);
        
        sleep(1);
        attempts++;
    }
    
    if (sem_empty == SEM_FAILED || sem_full == SEM_FAILED) {
        const char* error_msg = "client: Не удалось открыть семафоры\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        exit(1);
    }

    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if (shm_fd == -1) {
        const char* error_msg = "client: Ошибка открытия shared memory\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        exit(1);
    }
    
    shared_memory* shm = mmap(0, sizeof(shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED) {
        const char* error_msg = "client: Ошибка mapping shared memory\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        exit(1);
    }

    const char* prompt = "Введите строки :\n";
    write(STDOUT_FILENO, prompt, strlen(prompt));

    char input_buffer[SHM_SIZE];
    int bytes_read;
    
    while (1) {
        bytes_read = read(STDIN_FILENO, input_buffer, SHM_SIZE - 1);
        
        if (bytes_read <= 0) {
            sem_wait(sem_empty);
            shm->data_size = -1;
            sem_post(sem_full);
            break;
        }

        sem_wait(sem_empty);
        
        memcpy(shm->data, input_buffer, bytes_read);
        shm->data_size = bytes_read;
        
        sem_post(sem_full);
        
        sem_wait(sem_empty);
        
        write(STDOUT_FILENO, shm->data, shm->data_size);
        
        sem_post(sem_empty);
    }

    munmap(shm, sizeof(shared_memory));
    close(shm_fd);
    sem_close(sem_empty);
    sem_close(sem_full);

    return 0;
}