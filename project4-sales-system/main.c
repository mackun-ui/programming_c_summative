#include <stdio.h>
#include "transaction.h"
#include "utils.h"

static void show_menu(void) {
    printf("\n===== SALES TRANSACTION MANAGEMENT SYSTEM =====\n");
    printf(" 1. Add New Transaction\n");
    printf(" 2. Display All Transactions\n");
    printf(" 3. Update Transaction\n");
    printf(" 4. Delete Transaction\n");
    printf(" 5. Process Sales Data\n");
    printf(" 6. Filter Transactions (Callbacks)\n");
    printf(" 7. Search Transactions\n");
    printf(" 8. Sort Transactions\n");
    printf(" 9. Generate Sales Report\n");
    printf("10. Save Transactions\n");
    printf("11. Load Transactions\n");
    printf(" 0. Exit\n");
}

int main(void) {
    TransactionList list;
    int choice;

    if (!list_init(&list)) {
        return 1;
    }

    do {
        show_menu();
        choice = read_int("Choose an option: ", 0, 11);
        switch (choice) {
            case 1: add_transaction(&list); break;
            case 2: display_all_transactions(&list); break;
            case 3: update_transaction(&list); break;
            case 4: delete_transaction(&list); break;
            case 0: printf("Goodbye.\n"); break;
            default: printf("This option is not available yet.\n");
        }
    } while (choice != 0);

    list_free(&list);
    return 0;
}