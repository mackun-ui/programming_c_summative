#ifndef TRANSACTION_H
#define TRANSACTION_H

#define MAX_PRODUCT 50
#define DATE_LEN 11
#define INITIAL_CAPACITY 4

/* one sales transaction */
typedef struct {
    int id;
    char product[MAX_PRODUCT];
    int quantity;
    float unitPrice;
    float total;
    char date[DATE_LEN];
} Transaction;

/* dynamic list of transactions */
typedef struct {
    Transaction *items;
    int count;
    int capacity;
} TransactionList;

/* function pointer types */
typedef float (*SalesOperation)(const TransactionList *list);
typedef int (*FilterCallback)(const Transaction *t, const void *criteria);
typedef int (*CompareCallback)(const Transaction *a, const Transaction *b);

/* list management */
int list_init(TransactionList *list);
void list_free(TransactionList *list);
int list_append(TransactionList *list, const Transaction *t);
Transaction *find_transaction_by_id(TransactionList *list, int id);

/* transaction management */
void add_transaction(TransactionList *list);
void display_all_transactions(const TransactionList *list);
void update_transaction(TransactionList *list);
void delete_transaction(TransactionList *list);

/* sales operations (function pointers) */
float op_total_revenue(const TransactionList *list);
float op_average_value(const TransactionList *list);
float op_highest_value(const TransactionList *list);
float op_lowest_value(const TransactionList *list);
float op_total_units(const TransactionList *list);
void process_sales_data(const TransactionList *list);

/* callbacks and filtering */
int cb_amount_above(const Transaction *t, const void *criteria);
int cb_product_matches(const Transaction *t, const void *criteria);
int cb_quantity_at_least(const Transaction *t, const void *criteria);
int count_matching(const TransactionList *list, FilterCallback cb, const void *criteria);
void apply_filter(const TransactionList *list, FilterCallback cb, const void *criteria);
void filter_transactions_menu(const TransactionList *list);

/* searching and sorting */
void search_transactions(const TransactionList *list);
int cmp_by_id(const Transaction *a, const Transaction *b);
int cmp_by_product(const Transaction *a, const Transaction *b);
int cmp_by_amount_asc(const Transaction *a, const Transaction *b);
int cmp_by_amount_desc(const Transaction *a, const Transaction *b);
void sort_with_callback(TransactionList *list, CompareCallback cmp);
void sort_transactions(TransactionList *list);

/* reports */
void generate_sales_report(const TransactionList *list);

#endif