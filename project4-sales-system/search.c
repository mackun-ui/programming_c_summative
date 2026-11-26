#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "transaction.h"
#include "utils.h"

/* manual case-insensitive "contains" check (no strstr). */
static int contains_ignore_case(const char *text, const char *pattern) {
    int tl = (int)strlen(text);
    int pl = (int)strlen(pattern);

    for (int i = 0; i + pl <= tl; i++) {
        int j = 0;
        while (j < pl && tolower((unsigned char)text[i + j]) == tolower((unsigned char)pattern[j])) {
            j++;
        }
        if (j == pl) {
            return 1;
        }
    }
    return 0;
}

/* linear search by ID, product name or transaction amount. */
void search_transactions(const TransactionList *list) {
    if (list->count == 0) {
        printf("No transactions recorded.\n");
        return;
    }

    printf("\nSearch by:\n 1. Transaction ID\n 2. Product name\n 3. Transaction amount\n");
    int option = read_int("Choose: ", 1, 3);
    int found = 0;

    if (option == 1) {
        int id = read_int("Transaction ID: ", 1, 999999);
        for (int i = 0; i < list->count; i++) {
            if (list->items[i].id == id) {
                print_transaction_header();
                print_transaction_row(&list->items[i]);
                found++;
            }
        }
    } else if (option == 2) {
        char query[MAX_PRODUCT];
        read_line("Product name: ", query, MAX_PRODUCT);
        for (int i = 0; i < list->count; i++) {
            if (contains_ignore_case(list->items[i].product, query)) {
                if (found == 0) {
                    print_transaction_header();
                }
                print_transaction_row(&list->items[i]);
                found++;
            }
        }
    } else {
        float amount = read_float("Transaction amount: ", 0.0f, 100000000.0f);
        for (int i = 0; i < list->count; i++) {
            float diff = list->items[i].total - amount;
            if (diff < 0.0f) {
                diff = -diff;
            }
            if (diff < 0.005f) {         
                if (found == 0) {
                    print_transaction_header();
                }
                print_transaction_row(&list->items[i]);
                found++;
            }
        }
    }

    if (found == 0) {
        printf("No matching transactions found.\n");
    } else {
        printf("%d match(es) found.\n", found);
    }
}