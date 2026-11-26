#include <stdio.h>
#include "student.h"
#include "utils.h"

static void show_menu(void) {
    printf("\n===== STUDENT RECORDS SYSTEM =====\n");
    printf(" 1. Register new student\n");
    printf(" 2. Display all students\n");
    printf(" 3. View student details\n");
    printf(" 4. Update student information\n");
    printf(" 5. Delete a student\n");
    printf(" 6. Add a course to a student\n");
    printf(" 7. Update marks for a course\n");
    printf(" 8. Display student performance\n");
    printf(" 9. Search students\n");
    printf("10. Sort students\n");
    printf("11. Academic performance report\n");
    printf("12. Save records to file\n");
    printf("13. Load records from file\n");
    printf(" 0. Exit\n");
}

int main(void) {
    StudentList list;
    int choice;

    if (!list_init(&list)) {
        return 1;
    }

    do {
        show_menu();
        choice = read_int("Choose an option: ", 0, 13);
        switch (choice) {
            case 1: register_student(&list); break;
            case 2: display_all_students(&list); break;
            case 3: display_student_details(&list); break;
            case 4: update_student_info(&list); break;
            case 5: delete_student(&list); break;
            case 6: add_course_to_student(&list); break;
            case 7: update_course_marks(&list); break;
            case 8: display_student_performance(&list); break;
            case 9: search_students(&list); break;
            case 10: sort_students(&list); break;
            case 0: printf("Goodbye.\n"); break;
            default: printf("This option is not available yet.\n");
        }
    } while (choice != 0);

    list_free(&list);
    return 0;
}