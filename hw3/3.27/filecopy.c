#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <linux/stat.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    int pipefd[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];
    ssize_t numBytes;
    int sourceFile, destFile;

    // Check for correct number of arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s source_file destination_file\n", argv[0]);
        exit(1);
    }

    // Create pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    // Create child process
    pid = fork();

    // Check for fork error
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    // Child and parent process code
    if (pid == 0) { // Child process
        close(pipefd[1]); // Close write end of pipe
        destFile = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC | S_IRUSR | S_IWUSR, 0644);
        if (destFile == -1) {
            perror("open");
            exit(1);
        }

        // Read from pipe and write to destination file
        while ((numBytes = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
            if (write(destFile, buffer, numBytes) != numBytes) {
                perror("write");
                exit(1);
            }
        }

        // Check for read error
        if (numBytes == -1) {
            perror("read");
            exit(1);
        }

        // Close file descriptors and exit
        close(pipefd[0]);
        close(destFile);
        exit(EXIT_SUCCESS);
    } else { // Parent process
        close(pipefd[0]); // Close read end of pipe
        sourceFile = open(argv[1], O_RDONLY);
        if (sourceFile == -1) {
            perror("open");
            exit(1);
        }

        // Read from source file and write to pipe
        while ((numBytes = read(sourceFile, buffer, BUFFER_SIZE)) > 0) {
            if (write(pipefd[1], buffer, numBytes) != numBytes) {
                perror("write");
                exit(1);
            }
        }

        // Check for read error
        if (numBytes == -1) {
            perror("read");
            exit(1);
        }

        // Close file descriptors and wait for child to finish
        close(sourceFile);
        close(pipefd[1]); // Close write end of pipe
        wait(NULL); // Wait for child to finish
        exit(EXIT_SUCCESS);
    }
}
