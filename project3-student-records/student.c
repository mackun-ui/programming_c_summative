#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>
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

/* prints the header line of the student table. */
static void print_table_header(void) {
    printf("\n%-8s %-25s %-22s %-5s %-8s %s\n", "ID", "Name", "Programme", "Year", "Courses", "Average");
}

/* prints one student as a table row. */
static void print_student_row(const Student *s) {
    char full[2 * MAX_NAME + 2];
    char avg[16];
    float a = student_average(s);

    snprintf(full, sizeof(full), "%s %s", s->firstName, s->lastName);
    if (a < 0.0f) {
        snprintf(avg, sizeof(avg), "N/A");
    } else {
        snprintf(avg, sizeof(avg), "%.1f%%", a);
    }
    printf("%-8d %-25s %-22s %-5d %-8d %s\n", s->id, full, s->programme, s->year, s->numCourses, avg);
}

void display_all_students(const StudentList *list) {
    if (list->count == 0) {
        printf("No students registered.\n");
        return;
    }
    print_table_header();
    for (int i = 0; i < list->count; i++) {
        print_student_row(&list->students[i]);
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

void update_student_info(StudentList *list) {
    Student *s = select_student(list);
    if (s == NULL) {
        return;
    }
    printf("Updating student %d (the ID cannot be changed).\n", s->id);
    read_line("New first name: ", s->firstName, MAX_NAME);
    read_line("New last name: ", s->lastName, MAX_NAME);
    read_line("New programme of study: ", s->programme, MAX_NAME);
    s->year = read_int("New year of study (1-6): ", 1, 6);
    printf("Student updated.\n");
}

void delete_student(StudentList *list) {
    int id = read_int("Enter student ID to delete: ", 1, 999999);
    int idx = find_index(list, id);
    if (idx < 0) {
        printf("Error: no student with ID %d.\n", id);
        return;
    }

    char answer[8];
    read_line("Delete this student? (y/n): ", answer, sizeof(answer));
    if (answer[0] != 'y' && answer[0] != 'Y') {
        printf("Cancelled.\n");
        return;
    }

    free(list->students[idx].courses);         
    for (int i = idx; i < list->count - 1; i++) {
        list->students[i] = list->students[i + 1];
    }
    list->count--;

    /* shrink the array with realloc when it is mostly empty */
    if (list->capacity > INITIAL_CAPACITY && list->count < list->capacity / 4) {
        int newCap = list->capacity / 2;
        Student *tmp = (Student *)realloc(list->students, newCap * sizeof(Student));
        if (tmp != NULL) {
            list->students = tmp;
            list->capacity = newCap;
        }
    }
    printf("Student deleted.\n");
}

/* manual case-insensitive "contains" check (no strstr/strcasestr). */
static int contains_ignore_case(const char *text, const char *pattern) {
    int tl = (int)strlen(text);
    int pl = (int)strlen(pattern);

    for (int i = 0; i + pl <= tl; i++) {
        int j = 0;
        while (j < pl && tolower((unsigned char)text[i + j]) == tolower((unsigned char)pattern[j])) {
            j++;
        }
        if (j == pl) {
            return 1;
        }
    }
    return 0;
}

/* linear search by ID, name (first or last) or programme. */
void search_students(const StudentList *list) {
    if (list->count == 0) {
        printf("No students registered.\n");
        return;
    }

    printf("\nSearch by:\n 1. Student ID\n 2. First or last name\n 3. Programme of study\n");
    int option = read_int("Choose: ", 1, 3);
    int found = 0;

    if (option == 1) {
        int id = read_int("Student ID: ", 1, 999999);
        for (int i = 0; i < list->count; i++) {
            if (list->students[i].id == id) {
                if (found == 0) {
                    print_table_header();
                }
                print_student_row(&list->students[i]);
                found++;
            }
        }
    } else {
        char query[MAX_NAME];
        read_line(option == 2 ? "Name to search: " : "Programme to search: ", query, MAX_NAME);
        for (int i = 0; i < list->count; i++) {
            const Student *s = &list->students[i];
            int match;
            if (option == 2) {
                match = contains_ignore_case(s->firstName, query) || contains_ignore_case(s->lastName, query);
            } else {
                match = contains_ignore_case(s->programme, query);
            }
            if (match) {
                if (found == 0) {
                    print_table_header();
                }
                print_student_row(s);
                found++;
            }
        }
    }

    if (found == 0) {
        printf("No matching students found.\n");
    } else {
        printf("%d match(es) found.\n", found);
    }
}

/* compares two students by the chosen key. negative: a first, positive: b first. */
static int compare_students(const Student *a, const Student *b, int key) {
    switch (key) {
        case 1:
            return a->id - b->id;
        case 2: {
            int r = strcasecmp(a->lastName, b->lastName);
            if (r != 0) {
                return r;
            }
            return strcasecmp(a->firstName, b->firstName);
        }
        case 3: {
            float x = student_average(a);
            float y = student_average(b);
            if (x < y) return -1;
            if (x > y) return 1;
            return 0;
        }
        case 4:
            return strcasecmp(a->programme, b->programme);
    }
    return 0;
}

/* manual insertion sort (no qsort). */
void sort_students(StudentList *list) {
    if (list->count < 2) {
        printf("Not enough students to sort.\n");
        return;
    }

    printf("\nSort by:\n 1. Student ID\n 2. Name (alphabetical)\n 3. Overall average\n 4. Programme of study\n");
    int key = read_int("Choose: ", 1, 4);
    printf(" 1. Ascending\n 2. Descending\n");
    int order = read_int("Choose: ", 1, 2);

    for (int i = 1; i < list->count; i++) {
        Student current = list->students[i];
        int j = i - 1;
        while (j >= 0) {
            int cmp = compare_students(&list->students[j], &current, key);
            if (order == 2) {
                cmp = -cmp;
            }
            if (cmp <= 0) {
                break;
            }
            list->students[j + 1] = list->students[j];
            j--;
        }
        list->students[j + 1] = current;
    }

    printf("Students sorted.\n");
    display_all_students(list);
}

void generate_academic_report(const StudentList *list) {
    if (list->count == 0) {
        printf("No students registered.\n");
        return;
    }

    int totalCourses = 0, withCourses = 0;
    int studentsPass = 0, studentsFail = 0;
    int coursesPassed = 0, coursesFailed = 0;
    float sumAvg = 0.0f;
    const Student *best = NULL;
    const Student *worst = NULL;
    float bestAvg = 0.0f, worstAvg = 0.0f;

    for (int i = 0; i < list->count; i++) {
        const Student *s = &list->students[i];
        totalCourses += s->numCourses;

        for (int k = 0; k < s->numCourses; k++) {
            float pct = (s->courses[k].marks / s->courses[k].maxMarks) * 100.0f;
            if (pct >= PASS_PERCENT) {
                coursesPassed++;
            } else {
                coursesFailed++;
            }
        }

        float avg = student_average(s);
        if (avg < 0.0f) {
            continue;                        
        }
        withCourses++;
        sumAvg += avg;
        if (best == NULL || avg > bestAvg) {
            best = s;
            bestAvg = avg;
        }
        if (worst == NULL || avg < worstAvg) {
            worst = s;
            worstAvg = avg;
        }
        if (avg >= PASS_PERCENT) {
            studentsPass++;
        } else {
            studentsFail++;
        }
    }

    printf("\n===== ACADEMIC PERFORMANCE REPORT =====\n");
    printf("Total registered students: %d\n", list->count);
    printf("Total courses registered: %d\n", totalCourses);
    printf("Students with courses: %d\n", withCourses);

    if (withCourses > 0) {
        printf("Overall average performance: %.2f%%\n", sumAvg / withCourses);
        printf("Highest average: %s %s (%.2f%%)\n", best->firstName, best->lastName, bestAvg);
        printf("Lowest average: %s %s (%.2f%%)\n", worst->firstName, worst->lastName, worstAvg);
        printf("Students passing (avg >= %.0f%%): %d\n", PASS_PERCENT, studentsPass);
        printf("Students failing (avg < %.0f%%) : %d\n", PASS_PERCENT, studentsFail);
        printf("Course results passed / failed: %d / %d\n", coursesPassed, coursesFailed);
    } else {
        printf("No course data yet, so averages are not available.\n");
    }

    printf("\nStudents per programme:\n");
    for (int i = 0; i < list->count; i++) {
        int seenBefore = 0;
        for (int j = 0; j < i; j++) {
            if (strcasecmp(list->students[i].programme, list->students[j].programme) == 0) {
                seenBefore = 1;
                break;
            }
        }
        if (seenBefore) {
            continue;
        }
        int count = 0;
        for (int j = 0; j < list->count; j++) {
            if (strcasecmp(list->students[i].programme, list->students[j].programme) == 0) {
                count++;
            }
        }
        printf("  %-25s %d\n", list->students[i].programme, count);
    }
}