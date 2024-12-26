#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>
#include <time.h>

// Constants
#define MAX_PIZZAS 3 // Maximum number of pizzas that can be in the oven (on shelf)
#define TOTAL_PIZZAS 10 // Total number of pizzas to be baked (produced)
#define SHM_NAME "/pizzeria_shelf" // Name of the shared memory object
#define SEM_MUTEX "/pizzeria_mutex" // Name of the mutex semaphore
#define SEM_FULL "/pizzeria_full" // Name of the full semaphore
#define SEM_EMPTY "/pizzeria_empty" // Name of the empty semaphore

// Share memory structure
typedef struct {
    int pizzas_count; // Current number of pizzas on sheéf
    int pizzas_produced; // Total number of pizzas produced so far 
    int pizzas_delivered; // Total number of pizzas delivered so far
} SharedShelf;

// Helper functions for random delays
static inline void random_production_delay(void) {
    sleep(rand() % 2 + 1); // 1 - 2 seconds
}

static inline void random_delivery_delay(void) {
    sleep(rand() % 3 + 1); // 1 - 3 seconds
}

#endif // COMMON_H
