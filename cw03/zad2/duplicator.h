#ifndef DUPLICATOR_LIB
#define DUPLICATOR_LIB

typedef enum Mode {
    MEM, COPY
} Mode;

void monitor_file(char *file_path, const char *archive_path, int interval, int timeout, Mode mode);

#endif