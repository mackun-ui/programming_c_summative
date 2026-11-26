#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include "analyzer.h"

/* stores a formatted error message in the thread's arguments. */
void set_error(ThreadArgs *a, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(a->errorMsg, sizeof(a->errorMsg), fmt, ap);
    va_end(ap);
}

/* "data/document1.txt" becomes "reports/document1_report.txt" */
static void make_report_name(const char *filename, char *out, size_t size) {
    const char *base = strrchr(filename, '/');
    base = (base != NULL) ? base + 1 : filename;

    char stem[MAX_PATH_LEN];
    strncpy(stem, base, sizeof(stem) - 1);
    stem[sizeof(stem) - 1] = '\0';

    char *dot = strrchr(stem, '.');
    if (dot != NULL) {
        *dot = '\0';
    }
    snprintf(out, size, "reports/%s_report.txt", stem);
}

/* prepares the arguments for one thread. */
void init_thread_args(ThreadArgs *a, int id, const char *filename, const char *keyword) {
    memset(a, 0, sizeof(*a));
    a->threadId = id;
    strncpy(a->filename, filename, sizeof(a->filename) - 1);
    a->keyword = keyword;
    make_report_name(filename, a->reportName, sizeof(a->reportName));
    a->status = STATUS_PENDING;
}

void free_thread_args(ThreadArgs *a) {
    free(a->matchLineNumbers);
    a->matchLineNumbers = NULL;
    a->matchCapacity = 0;
}

/* counts the keyword in a line, ignoring case (manual search, no strstr). */
int count_occurrences(const char *text, const char *keyword) {
    size_t tl = strlen(text);
    size_t kl = strlen(keyword);
    int count = 0;

    if (kl == 0) {
        return 0;
    }
    for (size_t i = 0; i + kl <= tl; ) {
        size_t j = 0;
        while (j < kl && tolower((unsigned char)text[i + j]) == tolower((unsigned char)keyword[j])) {
            j++;
        }
        if (j == kl) {
            count++;
            i += kl;
        } else {
            i++;
        }
    }
    return count;
}