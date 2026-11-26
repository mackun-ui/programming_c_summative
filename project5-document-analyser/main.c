#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "analyzer.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <keyword> <file1> [file2 ...]\n", argv[0]);
        return 1;
    }
    const char *keyword = argv[1];
    int numFiles = argc - 2;

    if (keyword[0] == '\0') {
        printf("Error: the search keyword cannot be empty.\n");
        return 1;
    }

    printf("===== MULTI-THREADED DOCUMENT ANALYZER =====\n\n");
    printf("Search Keyword: %s\n", keyword);
    printf("Number of Input Files: %d\n\n", numFiles);

    ThreadArgs *args = (ThreadArgs *)calloc(numFiles, sizeof(ThreadArgs));
    pthread_t *threads = (pthread_t *)malloc(numFiles * sizeof(pthread_t));
    int *created = (int *)calloc(numFiles, sizeof(int));
    if (args == NULL || threads == NULL || created == NULL) {
        printf("Error: memory allocation failed.\n");
        free(args);
        free(threads);
        free(created);
        return 1;
    }

    /* prepare one set of arguments per file */
    for (int i = 0; i < numFiles; i++) {
        init_thread_args(&args[i], i + 1, argv[i + 2], keyword);
    }

    /* create one thread per file */
    for (int i = 0; i < numFiles; i++) {
        int rc = pthread_create(&threads[i], NULL, process_file, &args[i]);
        if (rc != 0) {
            args[i].status = STATUS_FAILED;
            set_error(&args[i], "thread creation failed: %s", strerror(rc));
            printf("[Main] ERROR: could not create thread %d: %s\n", i + 1, strerror(rc));
        } else {
            created[i] = 1;
            printf("[Main] Thread %d created for %s\n", i + 1, argv[i + 2]);
        }
    }

    /* wait for every thread that was created */
    for (int i = 0; i < numFiles; i++) {
        if (created[i]) {
            pthread_join(threads[i], NULL);
        }
    }

    display_results(args, numFiles);

    int failed = 0;
    for (int i = 0; i < numFiles; i++) {
        if (args[i].status != STATUS_SUCCESS) {
            failed++;
        }
        free_thread_args(&args[i]);
    }
    free(args);
    free(threads);
    free(created);

    return (failed == 0) ? 0 : 2;
}