#include <stdio.h>
#include "transaction.h"
#include "utils.h"

void generate_sales_report(const TransactionList *list) {
    if (list->count == 0) {
        printf("No transactions recorded.\n");
        return;
    }

    printf("\n===== SALES REPORT =====\n");
    printf("Total number of transactions : %d\n", list->count);
    printf("Total revenue                : %.2f\n", op_total_revenue(list));
    printf("Average transaction value    : %.2f\n", op_average_value(list));
    printf("Highest transaction value    : %.2f\n", op_highest_value(list));
    printf("Lowest transaction value     : %.2f\n", op_lowest_value(list));
    printf("Total quantity sold          : %.0f\n", op_total_units(list));

    /* revenue for one product, using the product callback */
    char product[MAX_PRODUCT];
    read_line("\nProduct for the revenue breakdown: ", product, MAX_PRODUCT);

    float revenue = 0.0f;
    int n = 0;
    for (int i = 0; i < list->count; i++) {
        if (cb_product_matches(&list->items[i], product)) {
            revenue += list->items[i].total;
            n++;
        }
    }

    if (n == 0) {
        printf("No transactions found for '%s'.\n", product);
    } else {
        printf("Revenue for %s: %.2f (%d transaction(s))\n", product, revenue, n);
    }
}