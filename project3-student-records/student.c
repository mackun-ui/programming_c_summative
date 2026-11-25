#include <stdio.h>
#include <stdlib.h>
#include "student.h"

/* allocates the student array. returns 1 on success, 0 on failure */
int list_init(StudentList *list) {
    list->students = (Student *)malloc(INITIAL_CAPACITY * sizeof(Student));
    if (list->students == NULL) {
        printf("Error: memory allocation failed.\n");
        return 0;
    }
    list->count = 0;
    list->capacity = INITIAL_CAPACITY;
    return 1;
}

/* frees every student's courses, then the student array */
void list_free(StudentList *list) {
    for (int i = 0; i < list->count; i++) {
        free(list->students[i].courses);
    }
    free(list->students);
    list->students = NULL;
    list->count = 0;
    list->capacity = 0;
}