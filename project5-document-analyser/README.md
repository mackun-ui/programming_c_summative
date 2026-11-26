# Multi-threaded Document Search and Analysis System

A C program that searches several text files at the same time using POSIX threads (pthreads). It starts one thread per file. Each thread counts the lines, finds a keyword (ignoring case), and writes its own report.

## Build and run
Run this on Linux (I used Ubuntu on WSL), from inside this folder:
```
gcc -Wall -Wextra -pthread -o document_analyzer *.c

./document_analyzer "IoT" data/document1.txt data/document2.txt data/document3.txt
```

The reports are written to `reports/`, for example `reports/document1_report.txt`.

## What each report contains
Input file name, search keyword, total lines, total keyword occurrences, number of matching lines, the line numbers where matches were found, and a processing status.

## Files
- `main.c`: reads the arguments, creates and joins the threads
- `analyzer.h` / `analyzer.c`: the thread arguments structure, the search, the report and the summary
- `data/`: three sample documents
- `reports/`: reports generated from the sample documents
- `docs/`: technical documentation and sample terminal output
- `screenshots/`: evidence of the program running