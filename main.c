#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define NEWLINE 1987986954
#define TEMPFILE "__temp_buf_file.txt"

#ifdef _WIN32
    #define OS "win"
#else
    #define OS "linux"
#endif

typedef struct {
    int from;
    int to;
} _range;

void initialize_flags(_range *flags)
{
    flags->from = -1;
    flags->to = -1;
}

int is_num(char c)
{
    return (c >= '0' && c <= '9');
}

int calc_total_lines(char *file)
{
    int fd = open(file, O_RDONLY, 0);

    if (fd == -1) {
        fprintf(stderr, "Error: %s does not exist\n", file);
        exit(1);
    }

    int c, total_lines = 0;

    while (read(fd, &c, 1))
        if (c == '\n')
            ++total_lines;

    if (total_lines)
        ++total_lines;

    return total_lines;
}

void get_line(char *s)
{
    unsigned int c;
    char *ptr_s = s;

    while ((c = getchar()) != '\n')
        *ptr_s++ = c;

    *ptr_s = '\0';

    return;
}

char *get_command(char *s, _range *flags, int total_lines)
{
    if (flags->from == -1 && *s == '$') {
        flags->from = (flags->to = total_lines);
        return ++s;
    }

    if (is_num(*s) || *s == '$') {
        flags->from = 0;
        while (*s != ' ' && *s != '\t')
            flags->from = flags->from * 10 + (*s++ - '0');

        while (*s == ' ' && *s++ == '\t')
            ;

        if (*s == '$') {
            flags->to = total_lines;
            ++s;
        }
        else if (is_num(*s)) {
            flags->to = 0;
            while (is_num(*s))
                flags->to = flags->to * 10 + (*s++ - '0');
        }
        else
            flags->to = flags->from;
    }
    else {
        fprintf(stderr, "%c: command not found\n", *s);
        ++s;
    }
    return s;
}

void copy_files(int to, int from)
{
    int c;
    while (read(from, &c, 1))
        write(to, &c, 1);

    return;
}

void print_line(char *file, int pointing_line, _range *flags)
{
    int line_from = (flags->from == -1) ? pointing_line : flags->from;
    int line_to = (flags->to == -1) ? pointing_line : flags->to;

    int fd = open(file, O_RDONLY, 0);

    int cur_line = 1;
    char c;

    while (cur_line < line_from && read(fd, &c, 1))
        if (c == '\n')
            ++cur_line;

    while (cur_line < line_to + 1 && read(fd, &c, 1)) {
        if (c == '\n')
            ++cur_line;
        write(1, &c, 1);
    }
    close(fd);

    return;
}

void append_text(char *file, int pointing_line)
{
    int fd1 = open(file, O_RDWR, 0);
    int fd2 = creat(TEMPFILE, 0);

    copy_files(fd2, fd1);

    close(fd1);
    close(fd2);
    int rfd1 = open(file, O_WRONLY, 0);
    int rfd2 = open(TEMPFILE, O_RDONLY, 0);

    int cur_line = 1;
    int c;
    while (cur_line < pointing_line && read(rfd2, &c, 1)) {
        if (c == NEWLINE)
            ++cur_line;
        write(rfd1, &c, 1);
    }

    while ((c = getchar()) != EOF)
        write(rfd1, &c, 1);

    while (read(rfd2, &c, 1))
        write(rfd1, &c, 1);

    close(rfd1);
    close(rfd2);

    remove(TEMPFILE);

    return;
}

void delete_line(char *file, int pointing_line, _range *flags)
{
    int line_from = (flags->from == -1) ? pointing_line : flags->from;
    int line_to = (flags->to == -1) ? pointing_line : flags->to;

    FILE *fptr1 = fopen(file, "r");
    FILE *fptr2 = fopen(TEMPFILE, "w");

    int cur_line = 1, c;

    while (cur_line < line_from && (c = getc(fptr1)) != EOF) {
        if (c == '\n')
            ++cur_line;
        putc(c, fptr2);
    }

    while (cur_line <= line_to && (c = getc(fptr1)) != EOF)
        if (c == '\n')
            ++cur_line;

    while ((c = getc(fptr1)) != EOF)
        putc(c, fptr2);

    fclose(fptr1);
    fclose(fptr2);

    FILE *f_ptr1 = fopen(file, "w");
    FILE *f_ptr2 = fopen(TEMPFILE, "r");

    while ((c = getc(f_ptr2)) != EOF)
        putc(c, f_ptr1);

    fclose(f_ptr1);
    fclose(f_ptr2);

    return;
}

char *get_num(char *command, int *cur_line, int total_lines)
{
    int prev_value = *cur_line;
    
    *cur_line = 0;

    while (is_num(*command))
        *cur_line = *cur_line * 10 + (*command++ - '0');
    
    if (*cur_line > total_lines) {
        fprintf(stderr, "Error: Number of lines exceeded total lines\n");
        *cur_line = prev_value;
    }

    return command;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: ed.exe <file.txt>\n");
        return 0;
    }

    _range *flags = (_range *) malloc(sizeof(_range));
    int cur_line = 1, total_lines = calc_total_lines(argv[1]);
    printf("Total lines: %d\n", total_lines);

    char *command = (char *) malloc(10 * sizeof(char));

    while (1) {
        initialize_flags(flags);
        printf("%d> ", cur_line);
        get_line(command);

        while (*command != '\0') {
            switch (*command) {
                case 'p':
                    ++command;
                    print_line(argv[1], cur_line, flags);
                    break;
                case 'a':
                    ++command;
                    append_text(argv[1], cur_line);
                    remove(TEMPFILE);
                    break;
                case ' ':
                    ++command;
                    break;
                case 'n':
                    ++command;
                    if (cur_line < total_lines)
                        ++cur_line;
                    break;
                case 'd':
                    ++command;
                    delete_line(argv[1], cur_line, flags);
                    remove(TEMPFILE);
                    break;
                case ':':
                    command = get_num(++command, &cur_line, total_lines);
                    break;
                case 'q':
                    exit(1);
                    break;
                default:
                    command = get_command(command, flags, total_lines);
                    break;
            }
        }
    }
    return 0;
}