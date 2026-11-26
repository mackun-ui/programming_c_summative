#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

/* reads a non-empty line that fits in dest. re-asks until the input is valid. */
void read_line(const char *prompt, char *dest, int size) {
    char tmp[256];

    while (1) {
        printf("%s", prompt);
        if (fgets(tmp, sizeof(tmp), stdin) == NULL) {
            printf("\nInput closed. Exiting.\n");
            exit(1);
        }

        int hasNewLine = (strchr(tmp, '\n') != NULL);
        tmp[strcspn(tmp, "\n")] = '\0';

        if (!hasNewLine) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("INput too long");
            continue;
        }
        if (tmp[0] == '\0') {
            printf("Input cannot be empty.\n");
            continue;
        }
        if (strchr(tmp, '|') != NULL) {
            printf("The '|' character is not allowed.\n");
            continue;
        }
        if ((int)strlen(tmp) >= size) {
            printf("Input too long (maximun %d character).\n", size - 1);
            continue;
        }
        strcpy(dest, tmp);
        return;
    }
}

/* reads a whole number between min and max. */
int read_int(const char *prompt, int min, int max) {
    char buf[64];
    char *end;

    while (1) {
        read_line(prompt, buf, sizeof(buf));
        long v = strtol(buf, &end, 10);
        if (*end == '\0' && v >= min && v <= max) {
            return (int)v;
        }
        printf("Invalid input. Enter a whole number from %d to %d.\n", min, max);
    }
}

/* reads a decimal number between min and max. */
float read_float(const char *prompt, float min, float max) {
    char buf[64];
    char *end;

    while (1) {
        read_line(prompt, buf, sizeof(buf));
        float v = strtof(buf, &end);
        if (*end == '\0' && v >= min && v <= max) {
            return v;
        }
        printf("Invalid input. Enter a number from %.1f to %.1f.\n", min, max);
    }
}