#include <stdio.h>
#include "analyzer.h"

int main(void) {
    ThreadArgs a;
    init_thread_args(&a, 1, "data/document1.txt", "IoT");
    process_file(&a);
    free_thread_args(&a);
    return 0;
}