#ifndef ANALYZER_H
#define ANALYZER_H

#include <stdio.h>

#define MAX_PATH_LEN 256
#define STATUS_PENDING 0
#define STATUS_SUCCESS 1
#define STATUS_FAILED -1

typedef struct {
    int threadId;
    char filename[MAX_PATH_LEN];
    char reportName[MAX_PATH_LEN + 64];
    const char *keyword;
    int totalLines;
    int totalOccurrences;
    int matchingLines;
    int *matchLineNumbers;      
    int matchCapacity;
    int status;
    char errorMsg[256];
} ThreadArgs;

void init_thread_args(ThreadArgs *a, int id, const char *filename, const char *keyword);
void free_thread_args(ThreadArgs *a);
void set_error(ThreadArgs *a, const char *fmt, ...);
int count_occurrences(const char *text, const char *keyword);
int search_file(FILE *fp, ThreadArgs *a);
void *process_file(void *arg);
void display_results(const ThreadArgs *args, int n);

#endif