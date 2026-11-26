#include <stdio.h>
#include "analyzer.h"

int main(void) {
    ThreadArgs a;
    init_thread_args(&a, 1, "data/document1.txt", "IoT");
    printf("Report file: %s\n", a.reportName);
    printf("Count: %d\n", count_occurrences("IoT and iot, then IOT.", "iot"));
    free_thread_args(&a);
    return 0;
}