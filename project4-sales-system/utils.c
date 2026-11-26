#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

/* returns 1 if text is a valid YYYY-MM-DD date. */
int is_valid_date(const char *text) {
    if (strlen(text) != 10 || text[4] != '-' || text[7] != '-') {
        return 0;
    }
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            continue;
        }
        if (!isdigit((unsigned char)text[i])) {
            return 0;
        }
    }

    int year = atoi(text);
    int month = atoi(text + 5);
    int day = atoi(text + 8);
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (year < 2000 || year > 2100 || month < 1 || month > 12) {
        return 0;
    }
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)) {
        days[1] = 29;
    }
    return day >= 1 && day <= days[month - 1];
}

/* reads a valid date into dest (dest must hold 11 characters). */
void read_date(const char *prompt, char *dest) {
    char buf[16];

    while (1) {
        read_line(prompt, buf, sizeof(buf));
        if (is_valid_date(buf)) {
            strcpy(dest, buf);
            return;
        }
        printf("Invalid date. Use the format YYYY-MM-DD (for example 2026-11-26).\n");
    }
}