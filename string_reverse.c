#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 256

int main() {
    int fd1[2]; // Pipe 1: Parent to Child
    int fd2[2]; // Pipe 2: Child to Parent

    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        // Parent process
        close(fd1[0]); // Close reading end of Pipe 1
        close(fd2[1]); // Close writing end of Pipe 2

        char message[BUFFER_SIZE];
        printf("Please type in a message: ");
        fgets(message, BUFFER_SIZE, stdin);

        // Write the message to Pipe 1
        write(fd1[1], message, strlen(message)+1);

        // Read the modified message from Pipe 2
        char modified_message[BUFFER_SIZE];
        read(fd2[0], modified_message, BUFFER_SIZE);
        printf("Modified message received from child: %s\n", modified_message);

        close(fd1[1]); // Close writing end of Pipe 1
        close(fd2[0]); // Close reading end of Pipe 2
    } else {
        // Child process
        close(fd1[1]); // Close writing end of Pipe 1
        close(fd2[0]); // Close reading end of Pipe 2

        char message[BUFFER_SIZE];

        // Read the message from Pipe 1
        read(fd1[0], message, BUFFER_SIZE);

        // Modify the message by reversing the case of each character
        for (int i = 0; message[i] != '\0'; i++) {
            if (islower(message[i])) {
                message[i] = toupper(message[i]);
            } else if (isupper(message[i])) {
                message[i] = tolower(message[i]);
            }
        }

        // Write the modified message to Pipe 2
        write(fd2[1], message, strlen(message)+1);

        close(fd1[0]); // Close reading end of Pipe 1
        close(fd2[1]); // Close writing end of Pipe 2
    }

    return 0;
}