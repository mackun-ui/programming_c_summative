#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transaction.h"
#include "utils.h"

/* allocates the transaction array. returns 1 on success, 0 on failure. */
int list_init(TransactionList *list) {
    list->items = (Transaction *)malloc(INITIAL_CAPACITY * sizeof(Transaction));
    if (list->items == NULL) {
        printf("Error: memory allocation failed.\n");
        return 0;
    }
    list->count = 0;
    list->capacity = INITIAL_CAPACITY;
    return 1;
}

void list_free(TransactionList *list) {
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

/* returns the index of the transaction with this ID, or -1. */
static int find_index(const TransactionList *list, int id) {
    for (int i = 0; i < list->count; i++) {
        if (list->items[i].id == id) {
            return i;
        }
    }
    return -1;
}

Transaction *find_transaction_by_id(TransactionList *list, int id) {
    int i = find_index(list, id);
    return (i < 0) ? NULL : &list->items[i];
}

/* adds a copy of t, doubling the array with realloc when it is full. */
int list_append(TransactionList *list, const Transaction *t) {
    if (list->count == list->capacity) {
        int newCap = list->capacity * 2;
        Transaction *tmp = (Transaction *)realloc(list->items, newCap * sizeof(Transaction));
        if (tmp == NULL) {
            printf("Error: memory allocation failed.\n");
            return 0;
        }
        list->items = tmp;
        list->capacity = newCap;
    }
    list->items[list->count] = *t;
    list->count++;
    return 1;
}

void add_transaction(TransactionList *list) {
    int id = read_int("Transaction ID: ", 1, 999999);
    if (find_index(list, id) >= 0) {
        printf("Error: transaction ID %d already exists.\n", id);
        return;
    }

    Transaction t;
    t.id = id;
    read_line("Product name: ", t.product, MAX_PRODUCT);
    t.quantity = read_int("Quantity sold: ", 1, 100000);
    t.unitPrice = read_float("Unit price: ", 0.01f, 1000000.0f);
    t.total = t.quantity * t.unitPrice;
    read_date("Date (YYYY-MM-DD): ", t.date);

    if (list_append(list, &t)) {
        printf("Transaction added. Total amount: %.2f\n", t.total);
    }
}

void display_all_transactions(const TransactionList *list) {
    if (list->count == 0) {
        printf("No transactions recorded.\n");
        return;
    }
    printf("\n%-6s %-22s %-6s %-12s %-12s %s\n", "ID", "Product", "Qty", "Unit price", "Total", "Date");
    for (int i = 0; i < list->count; i++) {
        const Transaction *t = &list->items[i];
        printf("%-6d %-22s %-6d %-12.2f %-12.2f %s\n", t->id, t->product, t->quantity,
               t->unitPrice, t->total, t->date);
    }
}