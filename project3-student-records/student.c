#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"
#include "utils.h"

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

/* returns the index of the student with this ID, or -1 if not found. */
static int find_index(const StudentList *list, int id) {
    for (int i = 0; i < list->count; i++) {
        if (list->students[i].id == id) {
            return i;
        }
    }
    return -1;
}

Student *find_student_by_id(StudentList *list, int id) {
    int i = find_index(list, id);
    return (i < 0) ? NULL : &list->students[i];
}

/* doubles the array with realloc when it is full. */
static int ensure_capacity(StudentList *list) {
    if (list->count < list->capacity) {
        return 1;
    }
    int newCap = list->capacity * 2;
    Student *tmp = (Student *)realloc(list->students, newCap * sizeof(Student));
    if (tmp == NULL) {
        printf("Error: memory allocation failed.\n");
        return 0;
    }
    list->students = tmp;
    list->capacity = newCap;
    return 1;
}

void register_student(StudentList *list) {
    int id = read_int("Student ID: ", 1, 999999);
    if (find_index(list, id) >= 0) {
        printf("Error: student ID %d already exists.\n", id);
        return;
    }
    if (!ensure_capacity(list)) {
        return;
    }

    Student *s = &list->students[list->count];
    s->id = id;
    read_line("Firstname: ", s->firstName, MAX_NAME);
    read_line("Last name: ", s->lastName, MAX_NAME);
    read_line("Programme of study: ", s->programme, MAX_NAME);
    s->year = read_int("Year of study (1-6): ", 1, 6);
    s->numCourses = 0;
    s->courses = NULL;
    list->count++;
    printf("Student registered.\n");
}

void display_all_students(const StudentList *list) {
    if (list->count == 0) {
        printf("No students registered.\n");
        return;
    }
    printf("\n%-8s %-25s %-22s %-5s %s\n", "ID", "Name", "Programme", "Year", "Courses");
    for (int i = 0; i < list->count; i++) {
        const Student *s = &list->students[i];
        char full[2 * MAX_NAME + 2];
        snprintf(full, sizeof(full), "%s %s", s->firstName, s->lastName);
        printf("%-8d %-25s %-22s %-5d %d\n", s->id, full, s->programme, s->year, s->numCourses);
    }
}