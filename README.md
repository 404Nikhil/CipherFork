## CipherFork: A Concurrent File Encryption Utility

CipherFork is a command-line tool written in C++ that encrypts or decrypts all files within a specified directory using a simple substitution cipher. This project serves as a practical example of file I/O, inter-process communication using shared memory, and robust process synchronization in a modern C++ application.

## Features

* **Concurrent Processing:** Uses a multiprocessing architecture (`fork()`) to handle multiple files at once, significantly speeding up operations in directories with many files.
* **Simple Interactive UI:** An easy-to-use prompt guides you through selecting a directory and an action.
* **Secure Key Management:** The encryption key is safely loaded from a `.env` file, keeping it separate from the source code.
* **Robust Synchronization:** Leverages POSIX semaphores to manage a process-safe producer-consumer queue, preventing race conditions.
* **Memory Safe IPC:** Uses POSIX shared memory (`mmap`) for high-speed communication and employs safe string functions (`strncpy`) to prevent buffer overflows.
* **Resilient by Design:** Includes comprehensive error checking for all system calls, ensuring the application fails gracefully instead of crashing on resource allocation errors.

## System Architecture

The application is built around a **producer-consumer model** using multiprocessing. The main process acts as the producer, scanning for files and adding them to a shared task queue. It then forks child processes, which act as consumers, each handling a single encryption or decryption task from the queue.

1.  **`main.cpp` (The Conductor):** The application's entry point. It takes user input, iterates through the target directory, and for each file, instructs the `ProcessManagement` module to submit a new task. It is also responsible for waiting for all child processes to complete before exiting.
2.  **`ProcessManagement` Module (The Task Manager):** This is the heart of the concurrent architecture. It is responsible for:
    * Safely setting up and tearing down a shared memory region and POSIX semaphores.
    * Forking new child processes to act as workers.
    * Implementing the logic for safely adding tasks (producing) and executing tasks (consuming).
3.  **`Encryption` Module (The Worker):** This module contains the core encryption/decryption logic. It's executed by each child process, which is responsible for opening the file, processing its content, and closing it.

## Multiprocessing and Synchronization

To allow the parent process (producer) and multiple child processes (consumers) to safely share the task queue, CipherFork uses a combination of shared memory and POSIX semaphores. This solves the classic computer science challenge of the **bounded-buffer producer-consumer problem**.

### Race Conditions and Memory Corruption

When multiple processes access a shared resource simultaneously, we risk critical bugs:

* **Race Conditions:** Two processes trying to modify the queue's state at the same time can lead to a corrupted queue, lost tasks, and unpredictable behavior.
* **Memory Corruption:** If a long file path is written into the shared memory without checking its length, it can write past its allocated buffer, corrupting adjacent data and causing random segmentation faults.

### Semaphores and Safe Memory Practices

**1. Semaphores as Locks and Signals:**
As seen in `ProcessManagement.cpp`, the application uses three named semaphores to orchestrate access to the shared queue. These are managed by the operating system and work reliably across different processes.

* **`mutexSemaphore`**: A binary semaphore (acting as a lock) that ensures only one process can be in a "critical section" (modifying the queue's `front` and `rear` pointers) at any given time.
* **`itemsSemaphore`**: A counting semaphore that tracks the number of tasks in the queue. Consumers wait on this, preventing them from trying to read from an empty queue.
* **`emptySlotsSemaphore`**: A counting semaphore that tracks available space. The producer waits on this, preventing it from adding tasks to a full queue.

**2. Robustness and Error Handling:**
A key feature of CipherFork's stability is its rigorous checking of system resources.

* **Initialization Checks:** The return values of `sem_open`, `shm_open`, and `mmap` are all checked. If any of these critical resources fail to be allocated by the OS, the program throws an exception and terminates immediately with a clear error message, preventing a crash later on.
* **Memory Safety:** The unsafe `strcpy` function is avoided. Instead, `strncpy` is used to copy file paths into the shared memory, guaranteeing that a long path can never cause a buffer overflow.

### The Logic Flow

Here is the sequence of operations for each process, which ensures a perfectly synchronized and safe workflow:

**Parent Process (Producer Logic in `main.cpp` and `submitToQueue`)**:

1.  Scans the directory for a file.
2.  Calls `submitToQueue` which waits for an empty slot (`sem_wait(emptySlotsSemaphore)`).
3.  Acquires the mutex lock (`sem_wait(mutexSemaphore)`).
4.  **Critical Section:** Safely copies the task string into the shared `tasks` array and updates the `rear` pointer.
5.  Releases the mutex lock (`sem_post(mutexSemaphore)`).
6.  Signals that a new item is available (`sem_post(itemsSemaphore)`).
7.  Calls `fork()` to create a new child process.
8.  The parent process **saves the child's PID** and continues its loop. It **does not** execute the task.
9.  After the directory scan is complete, the parent waits for all saved child PIDs to exit.

**Child Process (Consumer Logic in `executeTask`)**:

1.  The child process begins execution immediately after the `fork()`.
2.  It calls `executeTask()`, which waits for an available item (`sem_wait(itemsSemaphore)`).
3.  Acquires the mutex lock (`sem_wait(mutexSemaphore)`).
4.  **Critical Section:** Reads a task from the shared `tasks` array and updates the `front` pointer.
5.  Releases the mutex lock (`sem_post(mutexSemaphore)`).
6.  Signals that a slot has been freed up (`sem_post(emptySlotsSemaphore)`).
7.  Executes the encryption/decryption on the retrieved task.
8.  Calls `exit(0)` to terminate itself.


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