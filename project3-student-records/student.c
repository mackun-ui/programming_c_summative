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

/* asks for an ID and returns the student, or NULL with an error message. */
static Student *select_student(StudentList *list) {
    int id = read_int("Enter student ID: ", 1, 999999);
    Student *s = find_student_by_id(list, id);
    if (s == NULL) {
        printf("Error: no student with ID %d.\n", id);
    }
    return s;
}

/* overall average % using marks and maximum marks. returns -1 if no courses. */
float student_average(const Student *s) {
    float marks = 0.0f, maxMarks = 0.0f;
    for (int i = 0; i < s->numCourses; i++) {
        marks += s->courses[i].marks;
        maxMarks += s->courses[i].maxMarks;
    }
    if (maxMarks <= 0.0f) {
        return -1.0f;
    }
    return (marks / maxMarks) * 100.0f;
}

void display_student_details(StudentList *list) {
    Student *s = select_student(list);
    if (s == NULL) {
        return;
    }
    printf("\n--- Student %d ---\n", s->id);
    printf("Name: %s %s\n", s->firstName, s->lastName);
    printf("Programme: %s\n", s->programme);
    printf("Year: %d\n", s->year);
    printf("Courses: %d\n", s->numCourses);
    for (int i = 0; i < s->numCourses; i++) {
        const Course *c = &s->courses[i];
        printf("  %-10s %-30s %6.1f / %.1f\n", c->code, c->name, c->marks, c->maxMarks);
    }
}

void add_course_to_student(StudentList *list) {
    Student *s = select_student(list);
    if (s == NULL) {
        return;
    }

    Course c;
    read_line("Course code: ", c.code, MAX_CODE);
    for (int i = 0; i < s->numCourses; i++) {
        if (strcmp(s->courses[i].code, c.code) == 0) {
            printf("Error: course %s is already registered for this student.\n", c.code);
            return;
        }
    }
    read_line("Course name: ", c.name, MAX_COURSE_NAME);
    c.maxMarks = read_float("Maximum marks: ", 1.0f, 1000.0f);
    c.marks = read_float("Marks obtained: ", 0.0f, c.maxMarks);

    Course *tmp = (Course *)realloc(s->courses, (s->numCourses + 1) * sizeof(Course));
    if (tmp == NULL) {
        printf("Error: memory allocation failed.\n");
        return;
    }
    s->courses = tmp;
    s->courses[s->numCourses] = c;
    s->numCourses++;
    printf("Course added.\n");
}

void update_course_marks(StudentList *list) {
    Student *s = select_student(list);
    if (s == NULL) {
        return;
    }
    if (s->numCourses == 0) {
        printf("This student has no courses yet.\n");
        return;
    }

    char code[MAX_CODE];
    read_line("Course code: ", code, MAX_CODE);
    for (int i = 0; i < s->numCourses; i++) {
        if (strcmp(s->courses[i].code, code) == 0) {
            Course *c = &s->courses[i];
            c->marks = read_float("New marks: ", 0.0f, c->maxMarks);
            printf("Marks updated.\n");
            return;
        }
    }
    printf("Error: course %s not found for this student.\n", code);
}

void display_student_performance(StudentList *list) {
    Student *s = select_student(list);
    if (s == NULL) {
        return;
    }
    printf("\n--- Academic performance: %s %s ---\n", s->firstName, s->lastName);
    if (s->numCourses == 0) {
        printf("No courses registered, so no average can be calculated.\n");
        return;
    }
    for (int i = 0; i < s->numCourses; i++) {
        const Course *c = &s->courses[i];
        float pct = (c->marks / c->maxMarks) * 100.0f;
        printf("%-10s %-28s %6.1f / %-6.1f %6.1f%%  %s\n", c->code, c->name,
               c->marks, c->maxMarks, pct, pct >= PASS_PERCENT ? "PASS" : "FAIL");
    }
    printf("Overall average: %.2f%%\n", student_average(s));
}