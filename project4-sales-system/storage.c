#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "storage.h"
#include "utils.h"

/*
 * File format (text, one transaction per line):
 *   id|product|quantity|unitPrice|total|date
 */

/* writes all transactions to a text file. returns 1 on success. */
int save_to_file(const TransactionList *list, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: cannot open '%s' for writing: %s\n", filename, strerror(errno));
        return 0;
    }

    for (int i = 0; i < list->count; i++) {
        const Transaction *t = &list->items[i];
        fprintf(fp, "%d|%s|%d|%.2f|%.2f|%s\n", t->id, t->product, t->quantity,
                t->unitPrice, t->total, t->date);
    }

    int failed = ferror(fp);
    if (fclose(fp) != 0) {
        failed = 1;
    }
    if (failed) {
        printf("Error: a problem occurred while writing '%s'.\n", filename);
        return 0;
    }
    return 1;
}

/* loads into a temporary list first. the current list is only replaced if the
 * whole file is valid. A missing file is not an error. */
int load_from_file(TransactionList *list, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        if (errno == ENOENT) {
            printf("No saved file found (%s). Starting with an empty list.\n", filename);
            return 1;
        }
        printf("Error: cannot open '%s': %s\n", filename, strerror(errno));
        return 0;
    }

    TransactionList temp;
    if (!list_init(&temp)) {
        fclose(fp);
        return 0;
    }

    char line[512];
    int lineNo = 0;
    int ok = 1;

    while (fgets(line, sizeof(line), fp) != NULL) {
        lineNo++;
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') {
            continue;
        }

        Transaction t;
        if (sscanf(line, "%d|%49[^|]|%d|%f|%f|%10[^|]", &t.id, t.product, &t.quantity,
                   &t.unitPrice, &t.total, t.date) != 6
            || t.id < 1 || t.quantity < 1 || t.unitPrice <= 0.0f
            || !is_valid_date(t.date)) {
            printf("Error: invalid transaction record at line %d.\n", lineNo);
            ok = 0;
            break;
        }
        if (find_transaction_by_id(&temp, t.id) != NULL) {
            printf("Error: duplicate transaction ID %d at line %d.\n", t.id, lineNo);
            ok = 0;
            break;
        }
        t.total = t.quantity * t.unitPrice;
        if (!list_append(&temp, &t)) {
            ok = 0;
            break;
        }
    }
    if (ferror(fp)) {
        printf("Error: a problem occurred while reading '%s'.\n", filename);
        ok = 0;
    }
    fclose(fp);

    if (!ok) {
        list_free(&temp);
        printf("Load cancelled. The current records are unchanged.\n");
        return 0;
    }

    list_free(list);
    *list = temp;
    printf("Loaded %d transaction(s) from %s.\n", list->count, filename);
    return 1;
}