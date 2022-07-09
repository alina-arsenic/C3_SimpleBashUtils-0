#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define buffer_size 4096

void file_output(FILE *file, int* options, int* lastLineBlank, int* lineNumber);
void get_options(int *options, int argc, char **argv);
void get_files(char **files, int* n, int argc, char **argv);

int main(int argc, char **argv) {

    int options[5];
    get_options(options, argc, argv);

    int files_count = 0;
    char *files[argc];
    get_files(files, &files_count, argc, argv);

    int lastLineBlank = 0, lineNumber = 1;
    if (files_count == 0) {
        file_output(stdin, options, &lastLineBlank, &lineNumber);
    } else {
        FILE *file = NULL;
        for (int i = 0; i < files_count; i++) {
            file = fopen(files[i], "r");
            if (file)
                file_output(file, options, &lastLineBlank, &lineNumber);
            else
                fprintf(stderr, "s21_cat: %s: No such file or directory\n", argv[i]);
            fclose(file);
        }
    }

    return 0;
}

void file_output(FILE *file, int* options, int* lastLineBlank, int* lineNumber) {

    char buffer[buffer_size];
    int length;
    while (fgets(buffer, buffer_size / 2 - 1, file)) {

        if (options[3]) {  // -s сжимает несколько смежных пустых строк
            length = strlen(buffer);
            int currentLineBlank = (length <= 1) ? 1 : 0;
            if (*lastLineBlank && currentLineBlank) {
                continue;
            }
            *lastLineBlank = currentLineBlank;
        }
        if (options[4]) {  // -t также отображает табы как ^I
            char rest[buffer_size]; rest[0] = 0;
            char *tab = strchr(buffer, '\t');
            while (tab) {
                if (tab[1]) {
                    length = strlen(tab);
                    for (int i = 0; i < length; i++) {
                        rest[i] = tab[i+1];
                    }
                }
                tab[0] = '^'; tab[1] = 'I'; tab[2] = 0;
                strcat(buffer, rest);
                tab = strchr(buffer, '\t');
            }
        }
        if (options[0]) {  // -b нумерует только непустые строки
            length = strlen(buffer);
            if (length > 1) {
                char *tmp = strdup(buffer);
                buffer[0] = '\0';
                sprintf(buffer, "%*d\t", 6,  *lineNumber);
                *lineNumber += 1;
                strcat(buffer, tmp);
            }
        } else if (options[2]) {  // -n нумерует все выходные строки
            char *tmp = strdup(buffer);
            buffer[0] = '\0';
            sprintf(buffer, "%*d\t", 6, *lineNumber);
            *lineNumber += 1;
            strcat(buffer, tmp);
        }
        if (options[1]) {  // -e также отображает символы конца строки как $
            length = strlen(buffer);
            buffer[length-1] = '$';
            buffer[length] = '\n';
            buffer[length+1] = 0;
        }

        fprintf(stdout, "%s", buffer);
    }
}

void get_options(int *options, int argc, char **argv) {

    for (int i = 0; i < 5; i++) options[i] = 0;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] == '-') {
            if (strcmp(argv[i], "--number-nonblank") == 0) {
                argv[i][2] = 0;
                options[0] = 1;
            } else if (strcmp(argv[i], "--number") == 0) {
                argv[i][2] = 0;
                options[2] = 1;
            } else if (strcmp(argv[i], "--squeeze-blank") == 0) {
                argv[i][2] = 0;
                options[3] = 1;
            } else {
                fprintf(stdin, "s21_cat: invalid option -- '%s'", argv[i]);
                exit(1);
            }
        }
    }

    char temp;
    while ((temp = getopt(argc, argv, "beEnstT-")) != -1) {
        switch(temp) {
            case 'b':
                options[0] = 1;
                break;
            case 'e':
                options[1] = 1;
                break;
            case 'E':
                options[1] = 1;
                break;
            case 'n':
                options[2] = 1;
                break;
            case 's':
                options[3] = 1;
                break;
            case 't':
                options[4] = 1;
                break;
            case 'T':
                options[4] = 1;
                break;
            case '-':
                break;
            default:
                fprintf(stdin, "s21_cat: invalid option -- '%c'", temp);
                exit(1);
        }
    }
}

void get_files(char **files, int* n, int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
            if (argv[i][0] != '-') {
                files[*n] = argv[i];
                *n += 1;
            }
        }
}
