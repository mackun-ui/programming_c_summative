# Student Academic Records and Performance Analytics System

A menu-driven C program that manages student records and course marks using structures, pointers, dynamic memory and file storage.

## Build and run
Run from inside this folder (the data file path is relative):
```
gcc -Wall -Wextra -o student_system *.c
./student_system
```

## Features
- Register, view, update and delete students (duplicate IDs are rejected)
- Add courses and update marks (marks are validated against the maximum)
- Manual search by ID, name or programme
- Manual sorting by ID, name, average or programme (no qsort)
- Academic performance report (pass mark: 50%)
- Records are saved to and loaded from `data/students.txt`

## Files
- `main.c`: menu and program flow
- `student.h` / `student.c`: structures and student operations
- `storage.h` / `storage.c`: saving and loading
- `utils.h` / `utils.c`: validated input
- `data/students.txt`: sample data
- `docs/`: technical documentation
- `screenshots/`: evidence of the program running