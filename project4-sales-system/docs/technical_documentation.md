# Technical Documentation

## Program structure
`main.c` shows the menu in a loop and calls one function for each option. The code is split into files by job: list management, operations, callbacks, search, sorting, reports and storage.

## Structures
- `Transaction`: ID, product, quantity, unit price, total amount and date.
- `TransactionList`: a pointer to a dynamic array of transactions, with a count and a capacity.
The total is always calculated as quantity times unit price, including when a transaction is updated or loaded.

## Function pointers
Three function pointer types are defined in `transaction.h`:
- `SalesOperation`: takes the list and returns a float.
- `FilterCallback`: takes one transaction and some criteria and returns true or false.
- `CompareCallback`: takes two transactions and returns a negative number, zero or a positive number.

In `operations.c`, a table links each menu choice to a function (total revenue, average, highest, lowest and total units). `process_sales_data()` reads the user's choice, gets the pointer from the table, and calls the operation through it, so the operation is chosen at runtime.

## Callbacks
`cb_amount_above`, `cb_product_matches` and `cb_quantity_at_least` are callbacks. They receive their criteria as a `const void *` and cast it to the right type. `apply_filter()` and `count_matching()` take a callback as an argument and call it for every transaction. This means the same loop works for any condition. In `sorting.c`, `sort_with_callback()` receives a comparison callback (`cmp_by_id`, `cmp_by_product`, `cmp_by_amount_asc` or `cmp_by_amount_desc`) and uses it to decide the order.

## Dynamic memory
`list_init()` allocates the array with `malloc()`. `list_append()` doubles it with `realloc()` when it is full. `delete_transaction()` shifts the remaining items and shrinks the array with `realloc()` when it is less than a quarter full. `list_free()` releases the memory when the program ends. Every allocation is checked for NULL.

## Searching and sorting
Search is a linear scan with a hand-written case-insensitive "contains" function. Sorting is an insertion sort. `qsort()` is not used.

## File handling
Transactions are saved as one text line each, with fields separated by `|`. At startup the file is loaded, and on exit it is saved. `load_from_file()` reads into a temporary list and only replaces the current records if the whole file is valid. A missing file is reported without crashing, and invalid lines are reported with their line number.

## Error handling
Duplicate IDs, negative or zero quantities, invalid prices and dates, empty or over-long input, invalid menu choices, searches with no results, an empty transaction list, memory allocation failures, file errors and division by zero in the average are all handled with a message and no crash.