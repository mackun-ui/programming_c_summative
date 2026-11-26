#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "storage.h"

/*
 * File format (text, one record per line):
 *   S|id|first|last|programme|year|numCourses
 *   C|code|name|marks|maxMarks      
 *   (numCourses lines follow each S line)
 */

/* writes all students and their courses to a text file. returns 1 on success. */
int save_to_file(const StudentList *list, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: cannot open '%s' for writing: %s\n", filename, strerror(errno));
        return 0;
    }

    for (int i = 0; i < list->count; i++) {
        const Student *s = &list->students[i];
        fprintf(fp, "S|%d|%s|%s|%s|%d|%d\n", s->id, s->firstName, s->lastName,
                s->programme, s->year, s->numCourses);
        for (int k = 0; k < s->numCourses; k++) {
            const Course *c = &s->courses[k];
            fprintf(fp, "C|%s|%s|%.2f|%.2f\n", c->code, c->name, c->marks, c->maxMarks);
        }
    }

    int failed = ferror(fp);
    if (fclose(fp) != 0) {
        failed = 1;
    }
    if (failed) {
        printf("Error: a problem occurred while writing '%s'.\n", filename);
        return 0;
    }
    return 1;
}

/* reads one student (header line already read) and its course lines into temp.
 * nn any problem it frees what it allocated and returns 0. */
static int read_student(FILE *fp, const char *header, StudentList *temp, int *lineNo) {
    Student s;
    char line[512];
    s.courses = NULL;

    if (sscanf(header, "S|%d|%49[^|]|%49[^|]|%49[^|]|%d|%d", &s.id, s.firstName,
               s.lastName, s.programme, &s.year, &s.numCourses) != 6
        || s.id < 1 || s.year < 1 || s.year > 6
        || s.numCourses < 0 || s.numCourses > 1000
        || find_student_by_id(temp, s.id) != NULL) {
        printf("Error: invalid or duplicate student record at line %d.\n", *lineNo);
        return 0;
    }

    if (s.numCourses > 0) {
        s.courses = (Course *)calloc(s.numCourses, sizeof(Course));
        if (s.courses == NULL) {
            printf("Error: memory allocation failed.\n");
            return 0;
        }
    }

    for (int k = 0; k < s.numCourses; k++) {
        Course *c = &s.courses[k];
        if (fgets(line, sizeof(line), fp) == NULL) {
            printf("Error: the file ended while reading courses (line %d).\n", *lineNo);
            free(s.courses);
            return 0;
        }
        (*lineNo)++;
        line[strcspn(line, "\r\n")] = '\0';

        if (sscanf(line, "C|%14[^|]|%59[^|]|%f|%f", c->code, c->name, &c->marks, &c->maxMarks) != 4
            || c->maxMarks <= 0.0f || c->marks < 0.0f || c->marks > c->maxMarks) {
            printf("Error: invalid course record at line %d.\n", *lineNo);
            free(s.courses);
            return 0;
        }
    }

    if (!list_append(temp, &s)) {
        free(s.courses);
        return 0;
    }
    return 1;
}

/* loads records into a temporary list first. the current list is only replaced
 * if the whole file is valid. a missing file is not an error. */
int load_from_file(StudentList *list, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        if (errno == ENOENT) {
            printf("No saved file found (%s). Starting with an empty list.\n", filename);
            return 1;
        }
        printf("Error: cannot open '%s': %s\n", filename, strerror(errno));
        return 0;
    }

    StudentList temp;
    if (!list_init(&temp)) {
        fclose(fp);
        return 0;
    }

    char line[512];
    int lineNo = 0;
    int ok = 1;

    while (fgets(line, sizeof(line), fp) != NULL) {
        lineNo++;
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') {
            continue;
        }
        if (!read_student(fp, line, &temp, &lineNo)) {
            ok = 0;
            break;
        }
    }
    if (ferror(fp)) {
        printf("Error: a problem occurred while reading '%s'.\n", filename);
        ok = 0;
    }
    fclose(fp);

    if (!ok) {
        list_free(&temp);
        printf("Load cancelled. The current records are unchanged.\n");
        return 0;
    }

    list_free(list);
    *list = temp;
    printf("Loaded %d student(s) from %s.\n", list->count, filename);
    return 1;
}