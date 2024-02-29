#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <ctype.h>

#define BUFFER_SIZE 256

void reverseCase(char *str) {
    while (*str) {
        if (isupper(*str))
            *str = tolower(*str);
        else if (islower(*str))
            *str = toupper(*str);
        str++;
    }
}

int main() {
    char buffer[BUFFER_SIZE];
    DWORD bytesRead, bytesWritten;

    // Read message from parent process
    ReadFile(GetStdHandle(STD_INPUT_HANDLE), buffer, BUFFER_SIZE, &bytesRead, NULL);
    printf("child read >%s<\n", buffer);

    // Reverse the case of each character
    reverseCase(buffer);

    // Send modified message back to parent process
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buffer, BUFFER_SIZE, &bytesWritten, NULL);
    printf("child write >%s<\n", buffer);

    return 0;
}