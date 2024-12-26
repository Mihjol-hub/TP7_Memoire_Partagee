## TP7 - Pizzeria Simulation with Shared Memory and Semaphores

## Overview

This project implements a pizzeria simulation using shared memory and semaphores for synchronization between two processes: a cook (`chef`) and a server (`serveur`). The main goal is to demonstrate the safe manipulation of shared memory and the correct utilization of semaphores to solve a classic producer-consumer problem.

## Project Components

*   **`Makefile`:** Build file to compile the `chef` and `serveur` programs.
*   **`common.h`:** Header file containing common definitions, including the shared data structure (`SharedShelf`), shared memory and semaphore names, and utility functions.
*   **`chef.c`:** Implementation of the cook process. The cook produces pizzas and places them on a "shelf" (shared memory). It uses semaphores to synchronize with the server and to prevent the shelf from overflowing.
*   **`serveur.c`:** Implementation of the server process. The server removes pizzas from the shelf (shared memory) and "delivers" them. It uses semaphores to synchronize with the cook and to avoid attempting to retrieve pizzas from an empty shelf.

## Operation

1.  The `chef` process creates a shared memory segment representing the pizza shelf and creates the necessary semaphores for synchronization.
2.  The `serveur` process connects to the existing shared memory segment and semaphores.
3.  The `chef` produces pizzas at a random rate and places them on the shared shelf, up to a maximum of `MAX_PIZZAS`. It uses the `sem_empty` semaphore to wait if the shelf is full and the `sem_mutex` semaphore to ensure exclusive access to the shelf. It signals pizza availability using the `sem_full` semaphore.
4.  The `serveur` removes pizzas from the shared shelf at a random rate and simulates delivery. It uses the `sem_full` semaphore to wait if there are no pizzas available and the `sem_mutex` semaphore to ensure exclusive access to the shelf. It signals available space on the shelf using the `sem_empty` semaphore.
5.  The `chef` produces a total of `TOTAL_PIZZAS`.
6.  The `serveur` continues delivering pizzas until all produced pizzas have been delivered.
7. Finally, the `serveur` process cleans up and removes the shared memory segment and semaphores.

## Compilation and Execution Instructions

1.  **Compilation:** Run the command `make`. This will compile `chef.c` and `serveur.c` and generate the executables `chef` and `serveur`.
2.  **Execution:**
    *   Open one terminal to execute the cook process: `./chef`
    *   Open another terminal to execute the server process: `./serveur`
    *   **Important:** Both programs need to run simultaneously to observe their interaction through shared memory and semaphores. Start `./chef` first, followed by `./serveur`.

## Code Structure

*   **Shared Memory:** Communication between processes is performed through a shared memory segment identified by `SHM_NAME` ("/pizzeria\_shelf"). The `SharedShelf` structure in `common.h` defines the shared data (count of pizzas on the shelf, total produced, and delivered).
*   **Semaphores:** Synchronization is achieved using three semaphores:
    *   `SEM_MUTEX` ("/pizzeria\_mutex"): Mutex semaphore for mutual exclusion when accessing the shared shelf.
    *   `SEM_FULL` ("/pizzeria\_full"): Counting semaphore indicating the number of pizzas available on the shelf.
    *   `SEM_EMPTY` ("/pizzeria\_empty"): Counting semaphore indicating the number of available empty slots on the shelf.

## Design Decisions

*   Counting semaphores were used to control access to the shared shelf and to signal the availability of pizzas and empty slots.
*   The `chef` process is responsible for creating the shared memory and semaphores, while the `serveur` process connects to existing resources.
*   Cleanup of shared resources (unlinking of shared memory and semaphores) is performed in the `serveur` process upon termination, as it is the last process to use them.

## Bibliography 
1. Introduction to IPC (Inter process communicastion) https://www.geeksforgeeks.org/inter-process-communication-ipc/ 
2. How to Set up Shared Memory in Your Linux and MacOS Programs. (shmget, shmat, shmdt, shmctl, ftok) https://www.youtube.com/watch?v=WgVSq-sgHOc
3. Official Linux system function documentation. https://www.google.com/search?q=share+memory&sitesearch=man7.org%2Flinux%2Fman-pages&sa=Search+online+pages