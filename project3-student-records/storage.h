#ifndef STORAGE_H
#define STORAGE_H

#include "student.h"

#define DATA_FILE "data/students.txt"

int save_to_file(const StudentList *list, const char *filename);
int load_from_file(StudentList *list, const char *filename);

#endif