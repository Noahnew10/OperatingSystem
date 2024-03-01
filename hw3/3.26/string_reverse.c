#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 256

int main() {
    int fd1[2], fd2[2]; // File descriptors for two pipes
    pid_t pid;

    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("Pipe failed");
        return 1;
    }

    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }

    if (pid > 0) { // Parent process
        char input[BUFFER_SIZE];
        printf("Please type in a message: ");
        fgets(input, BUFFER_SIZE, stdin);

        close(fd1[0]); // Close reading end of first pipe

        // Write input to first pipe
        write(fd1[1], input, strlen(input) + 1);
        close(fd1[1]);

        close(fd2[1]); // Close writing end of second pipe

        // Read modified message from child process
        char output[BUFFER_SIZE];
        read(fd2[0], output, BUFFER_SIZE);
        printf("Parent read >%s<\n", output);
        close(fd2[0]);
    } else { // Child process
        close(fd1[1]); // Close writing end of first pipe

        // Read input from parent process
        char input[BUFFER_SIZE];
        read(fd1[0], input, BUFFER_SIZE);
        printf("Child read >%s<\n", input);

        // Reverse the case of each character
        for (int i = 0; input[i] != '\0'; i++) {
            if (islower(input[i])) {
                input[i] = toupper(input[i]);
            } else if (isupper(input[i])) {
                input[i] = tolower(input[i]);
            }
        }

        close(fd1[0]);

        close(fd2[0]); // Close reading end of second pipe

        // Write modified message to parent process
        write(fd2[1], input, strlen(input) + 1);
        close(fd2[1]);
    }

    return 0;
}