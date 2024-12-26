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
        close(shm_fd); // Close shared memory file descriptor
    }
    if (sem_mutex != SEM_FAILED) {
        sem_close(sem_mutex);
        
    }
    if (sem_full != SEM_FAILED) {
        sem_close(sem_full);
    
    }
    if (sem_empty != SEM_FAILED) {
        sem_close(sem_empty);
    
    }
    // shm_unlink(SHM_NAME);

    printf(" Chef Resources cleaned up successfully\n");
}

// Initialize shared memory - Creates or opens the shared memory segment for the pizza shelf.
static int init_shared_memory(void) {
    // Try to create the shared memory segment. Exclusive creation fails if it already exists.
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (shm_fd == -1) {
        if (errno == EEXIST) {
            printf("Shared memory already exists, opening again...\n");
            // If it exists, just open it for read-write.
            shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
            if (shm_fd == -1) {
                perror("shm_open (existing)");
                return -1;
            }
            // Map the existing shared memory segment into the process's address space.
            shelf = mmap(NULL, sizeof(SharedShelf), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
            if (shelf == MAP_FAILED) {
                perror("mmap (existing)");
                cleanup();
                return -1;
            }
            printf("Shared memory opened and mapped successfully...\n");
        } else {
            perror("shm_open");
            return -1;
        }
    } else {
        printf("Creating new shared memory segement...\n");
        // If creation was successful, set the size of the shared memory segment.
        if (ftruncate(shm_fd, sizeof(SharedShelf)) == -1) {
            perror("ftruncate");
            return -1;
        }
        // Map the newly created shared memory into the process's address space.
        shelf = mmap(NULL, sizeof(SharedShelf), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (shelf == MAP_FAILED) {
            perror("mmap");
            cleanup();
            return -1;
        }
        // Initialize the shared shelf state. 
        shelf->pizzas_count = 0;
        shelf->pizzas_produced = 0;
        shelf->pizzas_delivered = 0;
        printf("Shared memory created and initialized.\n");
    }

    return 0;
}

// TODO: Initialize semaphores
// Function to Initialize semaphores - Creates and initializes the semaphores for synchronization.
static int init_semaphores() {
    // Create mutex semaphore, initialized to 1 (unlocked).
    sem_mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0666, 1);
    if (sem_mutex == SEM_FAILED) {
        if (errno != EEXIST) {
            perror("sem_open for sem_mutex failed");
            cleanup();
            return -1;
        }
        // If the semaphore already exists, just open it.
        sem_mutex = sem_open(SEM_MUTEX, 0);
        if (sem_mutex == SEM_FAILED) {
            perror("sem_open for sem_mutex failed because it exists");
            cleanup();
            return -1;
        }
    }
    // Create empty semaphore, initialized to MAX_PIZZAS (all slots initially empty).
    sem_full = sem_open(SEM_FULL, O_CREAT | O_EXCL, 0666, 0);
    if (sem_full == SEM_FAILED) {
        if (errno != EEXIST) {
            perror("sem_open for sem_full failed");
            cleanup();
            return -1;
        }
        // If it exists, just open it.
        sem_full = sem_open(SEM_FULL, 0);
        if (sem_full == SEM_FAILED) {
            perror("sem_open for sem_full failed because it exists");
            cleanup();
            return -1;
        }
    }

    sem_empty = sem_open(SEM_EMPTY, O_CREAT | O_EXCL, 0666, MAX_PIZZAS);
    if (sem_empty == SEM_FAILED) {
        if (errno != EEXIST) {
            perror("sem_open for sem_empty failed");
            cleanup();
            return -1;
        }
        sem_empty = sem_open(SEM_EMPTY, 0);
        if (sem_empty == SEM_FAILED) {
            perror("sem_open for sem_empty failed because it exists");
            cleanup();
            return -1;
        }
    }

    return 0;
}

int main(void) {
    printf("Chef program starting...\n");

    // Initialize random seed for production delay simulation
    srand(time(NULL));

    // Register cleanup function to be called on normal process termination
    if (atexit(cleanup) != 0) {
        fprintf(stderr, "Cannot register cleanup function\n");
        return EXIT_FAILURE;
    }

    // TODO: Initialize shared memory for the pizza shelf
    if (init_shared_memory() == -1) {
        return EXIT_FAILURE;
    }

    printf("Shared memory initialized successfully\n");

   // Initialize semaphores for synchronization with the server 
    if (init_semaphores() == -1) {
        return EXIT_FAILURE;
    }

    printf("Semaphores initialized successfully\n");

    // Main loop for pizza production until the target number ir reached
    while (shelf->pizzas_produced < TOTAL_PIZZAS) {

        sem_wait(sem_empty); // Wait if the shelves is full (no empty slots)
        sem_wait(sem_mutex); // Acquire mutex to safely access the shared shelf

        random_production_delay(); // Simulate pizza production

        shelf->pizzas_count++;
        shelf->pizzas_produced++;

        printf("Chef made a pizza ... Total: %d Pizzas, in Shelf %d \n", shelf->pizzas_produced, shelf->pizzas_count);
        sem_post(sem_mutex); // Signal critical operation is done
        sem_post(sem_full); // Signals one slot less full

        //Log the current state of the shelf
        printf(" Log [Chef] count %d - produced %d - deliver %d \n", shelf->pizzas_count, shelf->pizzas_produced, shelf->pizzas_delivered);

    }

    // TODO: implement pizza production logic
    // TODO: Cleanup resources

    return EXIT_SUCCESS;
}
