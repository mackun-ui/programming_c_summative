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

/* remembers a matching line number, growing the array with realloc. */
static int record_match(ThreadArgs *a, int lineNo) {
    if (a->matchingLines == a->matchCapacity) {
        int newCap = (a->matchCapacity == 0) ? 8 : a->matchCapacity * 2;
        int *tmp = (int *)realloc(a->matchLineNumbers, newCap * sizeof(int));
        if (tmp == NULL) {
            set_error(a, "memory allocation failed");
            return 0;
        }
        a->matchLineNumbers = tmp;
        a->matchCapacity = newCap;
    }
    a->matchLineNumbers[a->matchingLines] = lineNo;
    a->matchingLines++;
    return 1;
}

/* reads the file line by line, counting lines, occurrences and matching lines. */
int search_file(FILE *fp, ThreadArgs *a) {
    char *line = NULL;
    size_t cap = 0;
    int lineNo = 0;

    while (getline(&line, &cap, fp) != -1) {
        lineNo++;
        int n = count_occurrences(line, a->keyword);
        if (n > 0) {
            a->totalOccurrences += n;
            if (!record_match(a, lineNo)) {  
                a->totalLines = lineNo;
                return 0;
            }
        }
    }
    free(line);
    a->totalLines = lineNo;

    if (ferror(fp)) {
        set_error(a, "error while reading the file");
        return 0;
    }
    return 1;
}

/* writes this thread's report file. Returns 1 on success. */
static int write_report(ThreadArgs *a) {
    FILE *out = fopen(a->reportName, "w");
    if (out == NULL) {
        if (a->errorMsg[0] == '\0') {
            set_error(a, "cannot create report file '%s': %s", a->reportName, strerror(errno));
        }
        return 0;
    }

    fprintf(out, "===== DOCUMENT ANALYSIS REPORT =====\n");
    fprintf(out, "Input file: %s\n", a->filename);
    fprintf(out, "Search keyword: %s\n", a->keyword);
    fprintf(out, "Total lines: %d\n", a->totalLines);
    fprintf(out, "Total occurrences: %d\n", a->totalOccurrences);
    fprintf(out, "Matching lines: %d\n", a->matchingLines);

    fprintf(out, "Line numbers: ");
    if (a->matchingLines == 0) {
        fprintf(out, "none\n");
    } else {
        for (int i = 0; i < a->matchingLines; i++) {
            fprintf(out, "%s%d", (i > 0) ? ", " : "", a->matchLineNumbers[i]);
        }
        fprintf(out, "\n");
    }

    if (a->status == STATUS_SUCCESS) {
        fprintf(out, "Status: SUCCESS%s\n",
                (a->matchingLines == 0) ? " (keyword not found)" : "");
    } else {
        fprintf(out, "Status: FAILED - %s\n", a->errorMsg);
    }

    int failed = ferror(out);
    if (fclose(out) != 0) {
        failed = 1;
    }
    if (failed) {
        if (a->errorMsg[0] == '\0') {
            set_error(a, "error while writing '%s'", a->reportName);
        }
        return 0;
    }
    return 1;
}

/* the function each thread runs: analyze one file and write its report. */
void *process_file(void *arg) {
    ThreadArgs *a = (ThreadArgs *)arg;
    int ok;

    printf("[Thread %d] Processing %s ...\n", a->threadId, a->filename);

    FILE *fp = fopen(a->filename, "r");
    if (fp == NULL) {
        set_error(a, "cannot open input file: %s", strerror(errno));
        ok = 0;
    } else {
        ok = search_file(fp, a);
        fclose(fp);
    }

    a->status = ok ? STATUS_SUCCESS : STATUS_FAILED;
    if (!write_report(a)) {
        a->status = STATUS_FAILED;
        ok = 0;
    }

    if (ok) {
        printf("[Thread %d] Successfully processed %s\n", a->threadId, a->filename);
    } else {
        printf("[Thread %d] ERROR processing %s: %s\n", a->threadId, a->filename, a->errorMsg);
    }
    return NULL;
}

/* prints one line per file, then the overall result. */
void display_results(const ThreadArgs *args, int n) {
    int failed = 0;

    printf("\n===== PROCESSING SUMMARY =====\n\n");
    for (int i = 0; i < n; i++) {
        const ThreadArgs *a = &args[i];
        if (a->status == STATUS_SUCCESS) {
            if (a->matchingLines == 0) {
                printf("%s: Completed (keyword not found)\n", a->filename);
            } else {
                printf("%s: Completed (%d occurrence(s) in %d line(s))\n",
                       a->filename, a->totalOccurrences, a->matchingLines);
            }
        } else {
            printf("%s: FAILED - %s\n", a->filename, a->errorMsg);
            failed++;
        }
    }

    printf("\n");
    if (failed == 0) {
        printf("All processing tasks completed.\n");
        printf("Reports generated successfully.\n");
    } else {
        printf("%d of %d file(s) could not be processed.\n", failed, n);
    }
}