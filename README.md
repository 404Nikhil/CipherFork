# ThreadCrypt: A Concurrent File Encryption Utility

ThreadCrypt is a command-line tool written in C++ that encrypts or decrypts all files within a specified directory using a simple substitution cipher. This project serves as a practical example of file I/O, inter-process communication using shared memory, and robust process synchronization in a modern C++ application.

## Features

* **Concurrent Processing:** Uses a multiprocessing architecture to handle multiple files at once, significantly speeding up operations in directories with many files.
* **Simple Interactive UI:** An easy-to-use prompt guides you through selecting a directory and an action.
* **Secure Key Management:** The encryption key is safely loaded from a `.env` file, keeping it separate from the source code.
* **Robust Synchronization:** Leverages POSIX semaphores to manage a thread-safe, process-safe producer-consumer queue, preventing race conditions and data corruption.
* **Efficient IPC:** Uses POSIX shared memory (`mmap`) for high-speed, low-overhead communication between the parent and child processes.

-----

## How It Works: System Architecture

The application is built around a **producer-consumer model** using multiprocessing. The main process acts as the producer, scanning for files and adding them to a shared task queue. It then forks child processes, which act as consumers, each handling a single encryption or decryption task from the queue.

1.  **`main.cpp` (The Conductor):** The application's entry point. It takes user input, iterates through the target directory, and for each file, instructs the `ProcessManagement` module to submit a new task.

2.  **`ProcessManagement` Module (The Task Manager):** This is the heart of the concurrent architecture. It is responsible for:
    * Setting up a shared memory region using `mmap` that contains the task queue.
    * Initializing the semaphores used for synchronization.
    * Forking new child processes to act as workers.
    * Implementing the logic for safely adding tasks (producing) and executing tasks (consuming).

3.  **`Encryption` Module (The Worker):** This module contains the core encryption/decryption logic. It's executed by each child process on a single task taken from the shared queue.

-----

## Deep Dive: Multiprocessing and Synchronization

To allow the parent process (producer) and multiple child processes (consumers) to safely share the task queue, it uses a combination of shared memory and POSIX semaphores. This solves the classic computer science challenge of the **bounded-buffer producer-consumer problem**.

### The Challenge: Race Conditions

When multiple processes access a shared resource (like our task queue) simultaneously, we risk a **race condition**. For example:
* The parent process might try to add a task to the queue at the exact same moment a child process is trying to remove one.
* Two child processes might try to take the same task at the same time.
* The parent might try to add a task to a full queue, overwriting an existing task that hasn't been processed yet.

These scenarios would lead to a corrupted queue, lost tasks, and unpredictable behavior. To prevent this, we must enforce mutual exclusion and manage the queue's state carefully.

### Semaphores as Locks and Signals

As seen in `ProcessManagement.cpp`, the application uses three semaphores to orchestrate access to the shared queue. A semaphore is a special integer variable that the operating system manages, which can be incremented (`sem_post`) or decremented (`sem_wait`) atomically.

1.  **The Mutex Lock (`mutexSemaphore`)**
    * **Purpose:** To ensure **mutual exclusion**. This semaphore acts as a simple lock. It guarantees that only one process can be inside a "critical section" (the code that modifies the queue's pointers `front`, `rear`, and `size`) at any given time.
    * **How it works:** It's initialized with a value of 1. Before accessing the queue, a process calls `sem_wait(mutexSemaphore)`, which decrements the value to 0. Since the value is now 0, any other process that calls `sem_wait` will be forced to sleep (block) until the first process is finished. Once the first process is done, it calls `sem_post(mutexSemaphore)`, which increments the value back to 1, waking up one of the waiting processes. This is the "lock" and "unlock" mechanism.

2.  **The Item Counter (`itemsSemaphore`)**
    * **Purpose:** To track the number of tasks currently in the queue. This prevents consumers from trying to read from an empty queue.
    * **How it works:** It's initialized to 0. When a consumer (child process) wants to take a task, it first calls `sem_wait(itemsSemaphore)`. If the queue is empty (value is 0), the process will block until the producer adds a task and calls `sem_post(itemsSemaphore)`, incrementing the count.

3.  **The Empty Slot Counter (`emptySlotsSemaphore`)**
    * **Purpose:** To track the number of available slots in the queue. This prevents the producer from adding tasks to a full queue.
    * **How it works:** It's initialized to `QUEUE_CAPACITY`. When the producer (parent process) wants to add a task, it first calls `sem_wait(emptySlotsSemaphore)`. If the queue is full (value is 0), the producer will block until a consumer removes a task and calls `sem_post(emptySlotsSemaphore)`, freeing up a slot.

### The Logic Flow

Here is the sequence of operations for each process, which ensures a perfectly synchronized and safe workflow:

**Parent Process (Producer Logic in `submitToQueue`)**:
1.  `sem_wait(emptySlotsSemaphore)`: Wait until there is an empty slot in the queue. If full, sleep.
2.  `sem_wait(mutexSemaphore)`: Acquire the lock to get exclusive access to the queue.
3.  **Critical Section:** Add the new task to the `tasks` array and update the `rear` pointer.
4.  `sem_post(mutexSemaphore)`: Release the lock.
5.  `sem_post(itemsSemaphore)`: Signal to any waiting consumers that a new item is available.
6.  `fork()`: Create a new child process to eventually consume a task.

**Child Process (Consumer Logic in `executeTask`)**:
1.  `sem_wait(itemsSemaphore)`: Wait until there is at least one item in the queue. If empty, sleep.
2.  `sem_wait(mutexSemaphore)`: Acquire the lock to get exclusive access to the queue.
3.  **Critical Section:** Read a task from the `tasks` array and update the `front` pointer.
4.  `sem_post(mutexSemaphore)`: Release the lock.
5.  `sem_post(emptySlotsSemaphore)`: Signal to the producer that a slot has been freed up.
6.  Execute the encryption/decryption on the retrieved task.

## Project Setup and Installation

### Prerequisites

  * G++ (GNU C++ Compiler)
  * make

### Installation Steps

1.  **Download the Code:**
    Clone or download the repository to your local machine.

2.  **Configure Environment:**
    Create a `.env` file in the project root by copying the example file and add your secret key.

    ```bash
    cp .env_example .env
    # Now, edit the .env file and set your ENCRYPTION_KEY
    ```

3.  **Compile:**
    Open your terminal in the project's root directory and run `make`.

    ```bash
    make
    ```

    This will generate an executable file named `encrypt_decrypt`.

-----

## Usage Guide

To run the program, simply execute the compiled file and follow the on-screen instructions.

1.  **Start the program:**

    ```bash
    ./encrypt_decrypt
    ```

2.  **Enter the directory path** when prompted. For example, if you want to process files in a folder named `my_files`, you would type:

    ```
    Enter the directory path: my_files
    ```

3.  **Enter the action** (`encrypt` or `decrypt`):

    ```
    Enter the action (encrypt/decrypt): encrypt
    ```

The application will then proceed to encrypt or decrypt every file in the `my_files` directory.