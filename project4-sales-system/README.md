# Sales Transaction Processing and Analysis System

A menu-driven C application that manages sales transactions for a small business. It demonstrates function pointers, callback functions, dynamic memory and file handling.

## Build and run
Run from inside this folder (the data file path is relative):
```
gcc -Wall -Wextra -o sales_system *.c
./sales_system
```

## Features
- Add, display, update and delete transactions (duplicate IDs are rejected)
- Process sales data with operations chosen at runtime (function pointers)
- Filter transactions with callbacks
- Manual search by ID, product or amount
- Manual sorting by ID, product or amount (no qsort)
- Sales report
- Transactions are saved to and loaded from `data/transactions.txt`

## Files
- `main.c`: menu and program flow
- `transaction.h` / `transaction.c`: structures, the dynamic list and transaction management
- `operations.c`: sales operations and the function pointer table
- `callbacks.c`: filter callbacks and the functions that call them
- `search.c`, `sorting.c`, `report.c`: searching, sorting and reports
- `storage.h` / `storage.c`: saving and loading
- `utils.h` / `utils.c`: validated input
- `data/`: sample transactions
- `docs/`: technical documentation
- `screenshots/`: evidence of the program running