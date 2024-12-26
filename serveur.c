#include "common.h"

// Global variables for cleanup 
static int shm_fd = -1; 
static SharedShelf *shelf = NULL; 
static sem_t *sem_mutex = SEM_FAILED; 
static sem_t *sem_full = SEM_FAILED; 
static sem_t *sem_empty = SEM_FAILED;

// Cleanup function 
void cleanup(void) {
    if (shelf != NULL) {
        munmap(shelf, sizeof(SharedShelf)); // Unmap shared memory segment
    }
    if (shm_fd != -1) {
        close(shm_fd);           
        shm_unlink(SHM_NAME); 
    }
    if (sem_mutex != SEM_FAILED) {
        sem_close(sem_mutex);   
        sem_unlink(SEM_MUTEX); 
    }
    if (sem_full != SEM_FAILED) {
        sem_close(sem_full);    
        sem_unlink(SEM_FULL); 
    }
    if (sem_empty != SEM_FAILED) {
        sem_close(sem_empty);
        sem_unlink(SEM_EMPTY); 
    }

    printf("Server Resources cleaned up successfully\n");
}

// Initialize shared memory - Opens and maps the existing shared memory segment.
static int init_shared_memory(void) {
    // Open existing shared memory segment
    shm_fd = shm_open(SHM_NAME, O_RDWR, 0);
    if (shm_fd == -1) {
        perror("shm_open");
        return -1;
    }

    // Map the shared memory segment into the process's address space
    shelf = mmap(NULL, sizeof(SharedShelf), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shelf == MAP_FAILED) {
        perror("mmap");
        cleanup();
        return -1;
    }

    return 0;
}

// TODO: Access semaphores
// Initialize semaphores - Opens the existing semaphores for synchronization.
static int init_semaphores() {
    // Open the existing mutex semaphore
    sem_mutex = sem_open(SEM_MUTEX, 0);
    if (sem_mutex == SEM_FAILED) {
        perror("sem_open mutex serveur");
        cleanup();
        return -1;
    }
    // Open the full semaphores 
    sem_full = sem_open(SEM_FULL, 0);
    if (sem_full == SEM_FAILED) {
        perror("sem_open full serveur");
        cleanup();
        return -1;
    }
    // Open the empty semaphores
    sem_empty = sem_open(SEM_EMPTY, 0);
    if (sem_empty == SEM_FAILED) {
        perror("sem_open empty serveur");
        cleanup();
        return -1;
    }

    return 0;
}

int main(void) {
    printf("Server program starting...\n");

    // Initialize random seed with different delay 
    srand(time(NULL) ^ getpid());

    // Register cleanup function
    if (atexit(cleanup) != 0) {
        fprintf(stderr, "Cannot register cleanup function\n");
        return EXIT_FAILURE;
    }

    // Initialize shared memory
    if (init_shared_memory() == -1) {
        return EXIT_FAILURE;
    }

    printf("Connected to shared memory successfully...\n");
    // Initialize semaphores by connecting to the existing semaphores.
    if (init_semaphores() == -1) {
        return EXIT_FAILURE;
    }

    printf("Connected to semaphores successfully...\n");

    // Main loop delivering pizzas as long as there are pizza to deliver
    while (shelf->pizzas_delivered < TOTAL_PIZZAS) {

        sem_wait(sem_full);
        sem_wait(sem_mutex); // wait for the access in critical section

        random_delivery_delay(); // Simulate pizza delivery  <- CAMBIO AQUÍ
        shelf->pizzas_count--;
        shelf->pizzas_delivered++;

        printf("Server delivered pizza ... total delivered :%d and %d pizzas left\n", shelf->pizzas_delivered, shelf->pizzas_count);
        sem_post(sem_mutex);  // critical zone unlocked
        sem_post(sem_empty); // one free space in the shelf.

    }

    return EXIT_SUCCESS;
}