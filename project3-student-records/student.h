#ifndef STUDENT_H
#define STUDENT_H

#define MAX_NAME 50
#define MAX_CODE 15
#define MAX_COURSE_NAME 60
#define INITIAL_CAPACITY 4

/* one course record for a student */
typedef struct {
    char code[MAX_CODE];
    char name[MAX_COURSE_NAME];
    float marks;
    float maxMarks;
} Course;

/* one student with a dynamic array of courses */
typedef struct {
    int id;
    char firstName[MAX_NAME];
    char lastName[MAX_NAME];
    char programme[MAX_NAME];
    int year;
    int numCourses;
    Course *courses;
} Student;

/* dynamic list of students */
typedef struct {
    Student *students;
    int count;
    int capacity;
} StudentList;

int list_init(StudentList *list);
void list_free(StudentList *list);

#define PASS_PERCENT 50.0f

Student *find_student_by_id(StudentList *list, int id);
void register_student(StudentList *list);
void display_all_students(const StudentList *list);
void display_student_details(StudentList *list);
void update_student_info(StudentList *list);
void delete_student(StudentList *list);
void add_course_to_student(StudentList *list);
void update_course_marks(StudentList *list);
void display_student_performance(StudentList *list);
float student_average(const Student *s);

void search_students(const StudentList *list);
void sort_students(StudentList *list);
void generate_academic_report(const StudentList *list);
int list_append(StudentList *list, const Student *s);

#endif