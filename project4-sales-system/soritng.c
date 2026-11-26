#include <stdio.h>
#include <strings.h>
#include "transaction.h"
#include "utils.h"

/* ---------- Comparison callbacks: each one matches the CompareCallback type ---------- */
/* Negative: a goes first, positive: b goes first, zero: equal. */

int cmp_by_id(const Transaction *a, const Transaction *b) {
    return a->id - b->id;
}

int cmp_by_product(const Transaction *a, const Transaction *b) {
    return strcasecmp(a->product, b->product);
}

int cmp_by_amount_asc(const Transaction *a, const Transaction *b) {
    if (a->total < b->total) return -1;
    if (a->total > b->total) return 1;
    return 0;
}

int cmp_by_amount_desc(const Transaction *a, const Transaction *b) {
    return cmp_by_amount_asc(b, a);  
}

/* manual insertion sort (no qsort). The callback decides the order. */
void sort_with_callback(TransactionList *list, CompareCallback cmp) {
    for (int i = 1; i < list->count; i++) {
        Transaction current = list->items[i];
        int j = i - 1;
        while (j >= 0 && cmp(&list->items[j], &current) > 0) {
            list->items[j + 1] = list->items[j];
            j--;
        }
        list->items[j + 1] = current;
    }
}

void sort_transactions(TransactionList *list) {
    if (list->count < 2) {
        printf("Not enough transactions to sort.\n");
        return;
    }

    printf("\nSort by:\n");
    printf(" 1. Transaction ID\n");
    printf(" 2. Product name\n");
    printf(" 3. Total amount (ascending)\n");
    printf(" 4. Total amount (descending)\n");
    int choice = read_int("Choose: ", 1, 4);

    CompareCallback cmp;      
    switch (choice) {
        case 1:  cmp = cmp_by_id; break;
        case 2:  cmp = cmp_by_product; break;
        case 3:  cmp = cmp_by_amount_asc; break;
        default: cmp = cmp_by_amount_desc; break;
    }

    sort_with_callback(list, cmp);
    printf("Transactions sorted.\n");
    display_all_transactions(list);
}