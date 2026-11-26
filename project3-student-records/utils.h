#ifndef UTILS_H
#define UTILS_H
void read_line(const char *prompt, char *dest, int size);
int read_int(const char *prompt, int min, int max);
float read_float(const char *prompt, float min, float max);

#endif