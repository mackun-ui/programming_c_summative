# Technical Documentation

## Structures
- `Course`: code, name, marks obtained and maximum marks.
- `Student`: ID, names, programme, year, number of courses, and a pointer to a dynamic array of `Course`.
- `StudentList`: pointer to a dynamic array of `Student`, with a count and a capacity.
A student owns their courses, and the list owns the students.

## Pointers
Functions take a pointer to the list or to one student and change it directly. For example, `select_student()` returns a `Student *` that points into the list, and `update_course_marks()` changes a `Course` through a pointer.

## Dynamic memory
- `list_init()` uses `malloc()` for the first 4 students.
- When the list is full, `ensure_capacity()` doubles it with `realloc()`.
- `add_course_to_student()` grows a student's course array by one with `realloc()`.
- `delete_student()` frees the student's courses, shifts the rest down, and shrinks the array with `realloc()` when it is less than a quarter full.
- `list_free()` frees every course array and then the list.
- Every allocation is checked for NULL, and an error message is shown if it fails.

## Searching and sorting
Search is a linear scan with a hand-written case-insensitive "contains" function. Sorting is an insertion sort with a comparison function, for four keys and both directions. `qsort()` is not used.

## File handling
Records are saved as text: an `S|...` line for each student, followed by one `C|...` line for each course. `load_from_file()` reads into a temporary list, rebuilding each student's course array with `calloc()`, and only replaces the current records if the whole file is valid. Missing files are reported without crashing, and bad lines are reported with their line number. The `|` character is refused in inputs because it is the separator.

## Academic calculations
A course is passed at 50% or more. A student's average is total marks divided by total maximum marks, times 100. Students with no courses have no average, which avoids division by zero.

## Error handling and testing
Tested: duplicate IDs, marks above the maximum, non-numeric input, invalid menu choices, empty and over-long input, deleting or updating a student who doesn't exist, students with no courses, a missing data file, and a corrupt load file.