# ThreadCrypt: Concurrent File Encryption Utility

ThreadCrypt is a command-line tool written in C++ that encrypts or decrypts all files within a specified directory. It uses a simple substitution cipher based on a secret key provided by the user. This project serves as a practical example of file I/O, process management concepts, and modern C++ application structure.

## Features

  * **Directory-Based Processing:** Encrypt or decrypt all files within a folder in one go.
  * **Simple Interactive UI:** An easy-to-use prompt guides you through the process.
  * **Secure Key Management:** The encryption key is safely loaded from a `.env` file, keeping it separate from the source code.
  * **Modular Design:** The code is organized into logical components for handling I/O, task management, and encryption, making it easy to understand and extend.
  * **Modern C++ Memory Management:** Uses `std::unique_ptr` and `std::move` for safe, automatic memory management, preventing memory leaks.

-----

## How It Works: System Architecture 

The application is broken down into several key components that work together to perform the file operations.

1.  **`main.cpp` (The Conductor):** This is the entry point of the application. It orchestrates the entire workflow by initializing and calling the other modules. It first uses the `IO` module to get user input, then creates a `Task` for each file, adds them to the `ProcessManagement` queue, and finally initiates the execution.

2.  **`IO` Module (Input/Output Handler):** This component is responsible for all interaction with the user and the file system.

      * `getDirectoryPathAndAction()`: Prompts the user to enter the target directory and the desired action (`encrypt`/`decrypt`).
      * `readDirectory()`: Scans the specified directory and returns a list of all filenames within it.

3.  **`Task` Module (The Work Order):** A simple class that represents a single unit of work. Each `Task` object contains two pieces of information:

      * The **action** to be performed (`ENCRYPT` or `DECRYPT`).
      * The **file path** of the target file.

4.  **`ProcessManagement` Module (The Task Manager):** This module manages the workflow. It maintains a queue of `Task` objects. Its `executeTasks()` method iterates through the queue, processing each task sequentially. For every task, it calls the core `executeEncryption` function to perform the actual file modification.

5.  **`Encryption` Module (The Worker):** This is the core logic engine. The `executeEncryption` function takes a task, reads the specified file character by character, applies a simple Caesar cipher (shifting the character's value by the secret key), and overwrites the file with the new content.

6.  **`ReadEnv` Module (Configuration):** A small utility that handles reading the `ENCRYPTION_KEY` from the `.env` file, ensuring that sensitive data is not hardcoded.

-----

## Memory Management: The Role of `unique_ptr` and `std::move`

This project uses modern C++ smart pointers to ensure safe and automatic memory management, completely avoiding manual `new` and `delete` calls.

  * **`std::unique_ptr`**: This is a smart pointer that provides exclusive ownership of a dynamically allocated object. When a `unique_ptr` goes out of scope, it automatically deletes the object it points to, which effectively prevents memory leaks.

  * **`std::move`**: Because a `unique_ptr` guarantees exclusive ownership, it cannot be copied. To transfer the ownership of the object from one `unique_ptr` to another, we use `std::move`. This makes the transfer of ownership an explicit and safe operation.

This pattern is used to manage the lifecycle of `Task` objects:

1.  **Creation in `main.cpp`**: For each file, a `Task` is created on the heap using `std::make_unique<Task>()`. A `unique_ptr` in `main` now owns this `Task`.

2.  **Ownership Transfer**: The `main` function calls `processManager.submitToQueue(std::move(task))`. The `std::move` transfers ownership of the `Task` object from the `main` function to the `ProcessManagement` module's `taskQueue`.

3.  **Execution in `ProcessManagement.cpp`**: Inside the `executeTasks` loop, ownership is moved again from the queue to a local `unique_ptr` named `taskToExecute`.

4.  **Automatic Cleanup**: At the end of each loop iteration, `taskToExecute` goes out of scope. Its destructor is called, which automatically frees the memory of the `Task` object it owns.

This entire process ensures that every `Task` object is correctly deallocated without any manual memory management, leading to safer and more robust code.

-----

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