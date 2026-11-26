#include <stdio.h>
#include <strings.h>
#include "transaction.h"
#include "utils.h"

/* ---------- Callbacks: each one matches the FilterCallback type ---------- */

/* criteria points to a float: the minimum amount */
int cb_amount_above(const Transaction *t, const void *criteria) {
    const float *min = (const float *)criteria;
    return t->total > *min;
}

/* criteria points to a string: the product name (not case-sensitive) */
int cb_product_matches(const Transaction *t, const void *criteria) {
    const char *name = (const char *)criteria;
    return strcasecmp(t->product, name) == 0;
}

/* criteria points to an int: the minimum quantity */
int cb_quantity_at_least(const Transaction *t, const void *criteria) {
    const int *min = (const int *)criteria;
    return t->quantity >= *min;
}

/* ---------- Functions that receive a callback and call it ---------- */

/* counts the transactions for which the callback returns true. */
int count_matching(const TransactionList *list, FilterCallback cb, const void *criteria) {
    int n = 0;
    for (int i = 0; i < list->count; i++) {
        if (cb(&list->items[i], criteria)) {
            n++;
        }
    }
    return n;
}

/* prints the transactions for which the callback returns true. */
void apply_filter(const TransactionList *list, FilterCallback cb, const void *criteria) {
    int found = 0;
    for (int i = 0; i < list->count; i++) {
        if (cb(&list->items[i], criteria)) {
            if (found == 0) {
                print_transaction_header();
            }
            print_transaction_row(&list->items[i]);
            found++;
        }
    }
    if (found == 0) {
        printf("No transactions match this filter.\n");
    } else {
        printf("%d transaction(s) match.\n", found);
    }
}

void filter_transactions_menu(const TransactionList *list) {
    if (list->count == 0) {
        printf("No transactions recorded.\n");
        return;
    }

    printf("\n===== FILTER TRANSACTIONS =====\n");
    printf("1. Transactions above a sales amount\n");
    printf("2. Transactions for a specific product\n");
    printf("3. Count transactions with at least a given quantity\n");
    int choice = read_int("Choose a filter: ", 1, 3);

    if (choice == 1) {
        float min = read_float("Minimum sales amount: ", 0.0f, 100000000.0f);
        apply_filter(list, cb_amount_above, &min);
    } else if (choice == 2) {
        char name[MAX_PRODUCT];
        read_line("Product name: ", name, MAX_PRODUCT);
        apply_filter(list, cb_product_matches, name);
    } else {
        int min = read_int("Minimum quantity: ", 1, 100000);
        int n = count_matching(list, cb_quantity_at_least, &min);
        printf("%d transaction(s) have a quantity of %d or more.\n", n, min);
    }
}