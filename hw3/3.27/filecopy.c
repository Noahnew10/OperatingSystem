#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUF_SIZE 4096

// Copy a file using a pipe
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create a pipe to transfer data from source file to destination file
    int pipefd[2];
    pid_t pid;
    char buffer[BUF_SIZE];
    ssize_t bytes_read;

    // Create a pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Create a child process
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // Child process
        close(pipefd[1]);  // Close unused write end

        // Open destination file for writing
        FILE *dest = fopen(argv[2], "w");
        if (dest == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        // Read from pipe and write to destination file
        while ((bytes_read = read(pipefd[0], buffer, BUF_SIZE)) > 0) {
            if (fwrite(buffer, 1, bytes_read, dest) != bytes_read) {
                perror("fwrite");
                exit(EXIT_FAILURE);
            }
        }

        // Check for read errors
        if (bytes_read == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        // Close destination file and pipe
        fclose(dest);
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    } else {  // Parent process
        close(pipefd[0]);  // Close unused read end

        // Open source file for reading
        FILE *source = fopen(argv[1], "r");
        if (source == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        // Read from source file and write to pipe
        while ((bytes_read = fread(buffer, 1, BUF_SIZE, source)) > 0) {
            if (write(pipefd[1], buffer, bytes_read) != bytes_read) {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }

        // Check for fread errors
        if (bytes_read == -1) {
            perror("fread");
            exit(EXIT_FAILURE);
        }

        // Close source file and pipe
        fclose(source);
        close(pipefd[1]);
        wait(NULL);  // Wait for child process to complete
        exit(EXIT_SUCCESS);
    }

    return 0;
}
