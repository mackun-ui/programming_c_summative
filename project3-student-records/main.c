#include <stdio.h>
#include "student.h"

int main(void) {
    StudentList list;

    if (!list_init(&list)) {
        return 1;
    }
    printf("List ready. Capacity: %d, students: %d\n", list.capacity, list.count);

    list_free(&list);
    return 0;
}