#include <stdio.h>
#include "transaction.h"
#include "utils.h"

/* ---------- Operations: each one matches the SalesOperation type ---------- */

float op_total_revenue(const TransactionList *list) {
    float sum = 0.0f;
    for (int i = 0; i < list->count; i++) {
        sum += list->items[i].total;
    }
    return sum;
}

float op_average_value(const TransactionList *list) {
    if (list->count == 0) {                  
        return 0.0f;
    }
    return op_total_revenue(list) / list->count;
}

float op_highest_value(const TransactionList *list) {
    if (list->count == 0) {
        return 0.0f;
    }
    float max = list->items[0].total;
    for (int i = 1; i < list->count; i++) {
        if (list->items[i].total > max) {
            max = list->items[i].total;
        }
    }
    return max;
}

float op_lowest_value(const TransactionList *list) {
    if (list->count == 0) {
        return 0.0f;
    }
    float min = list->items[0].total;
    for (int i = 1; i < list->count; i++) {
        if (list->items[i].total < min) {
            min = list->items[i].total;
        }
    }
    return min;
}

float op_total_units(const TransactionList *list) {
    float units = 0.0f;
    for (int i = 0; i < list->count; i++) {
        units += list->items[i].quantity;
    }
    return units;
}

/* ---------- Table of operations: a name and a function pointer ---------- */
typedef struct {
    const char *label;
    SalesOperation op;
} OperationEntry;

static const OperationEntry operations[] = {
    {"Total revenue", op_total_revenue},
    {"Average transaction value", op_average_value},
    {"Highest-value transaction", op_highest_value},
    {"Lowest-value transaction", op_lowest_value},
    {"Total units sold", op_total_units}
};
#define NUM_OPERATIONS (int)(sizeof(operations) / sizeof(operations[0]))

/* lets the user pick an operation at runtime and calls it through a pointer. */
void process_sales_data(const TransactionList *list) {
    if (list->count == 0) {
        printf("No transactions recorded.\n");
        return;
    }

    printf("\n===== PROCESS SALES DATA =====\n");
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        printf(" %d. %s\n", i + 1, operations[i].label);
    }
    int choice = read_int("Choose an operation: ", 1, NUM_OPERATIONS);

    SalesOperation op = operations[choice - 1].op;  
    float result = op(list);                      

    if (choice == 5) {
        printf("%s: %.0f\n", operations[choice - 1].label, result);
    } else {
        printf("%s: %.2f\n", operations[choice - 1].label, result);
    }

    /* for highest and lowest, also show which transaction it is */
    if (choice == 3 || choice == 4) {
        for (int i = 0; i < list->count; i++) {
            if (list->items[i].total == result) {
                print_transaction_header();
                print_transaction_row(&list->items[i]);
                break;
            }
        }
    }
}