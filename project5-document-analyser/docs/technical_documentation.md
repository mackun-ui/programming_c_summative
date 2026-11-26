# Technical Documentation

## Program structure
`main.c` creates one thread for each input file, waits for all of them, and shows a summary. The work is split into functions in `analyzer.c`:
- `init_thread_args()`: prepares the arguments for one thread
- `process_file()`: the function every thread runs
- `search_file()` and `count_occurrences()`: search for the keyword and count occurrences
- `write_report()`: writes the report file of one thread
- `display_results()`: shows the processing summary
- `set_error()` and `free_thread_args()`: error messages and cleanup

## Threads
`main()` allocates an array of `ThreadArgs` structures, one for each file. Each structure holds the file name, the keyword, the report name, the results, a status and an error message. `pthread_create()` starts a thread for each file and passes it a pointer to its own structure. After starting all the threads, `main()` calls `pthread_join()` on each thread that was created, so the program never ends before the threads finish.

## Why no synchronization is needed
Each thread works only on its own `ThreadArgs` structure and writes to its own report file. The only shared data is the keyword, which is never changed. Because no thread writes to data that another thread uses, there are no race conditions, so mutexes are not needed. The terminal messages come from `printf()`, which is safe to call from several threads, but the order of the messages can change from one run to the next.

## Searching
Each thread reads its file one line at a time with `getline()`, so lines of any length work. `count_occurrences()` compares characters without regard to case and moves forward by the keyword length after each match. The line numbers of matching lines are kept in a dynamic array that grows with `realloc()`.

## Reports and status
Each thread writes `reports/<name>_report.txt`. The status is SUCCESS, SUCCESS (keyword not found), or FAILED with the reason. A failed thread still tries to write a report, so the failure is recorded.

## Thread status monitoring
The main program prints when each thread is created. Each thread prints when it starts processing a file, and when it finishes or fails. `display_results()` prints the final status of every file.

## Error handling
- Missing or unreadable input files are reported for that file, and the other threads continue.
- Report files that cannot be created or written are reported.
- A keyword that is not found produces a report saying so.
- A thread that cannot be created is reported, and the threads that were created are still joined.
- Missing arguments and an empty keyword show a message and stop the program.
- Memory allocation failures are checked.
- The program returns 0 when everything succeeds and 2 when any file failed.