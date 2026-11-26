#ifndef STORAGE_H
#define STORAGE_H

#include "transaction.h"

#define DATA_FILE "data/transactions.txt"

int save_to_file(const TransactionList *list, const char *filename);
int load_from_file(TransactionList *list, const char *filename);

#endif