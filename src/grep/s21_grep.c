#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <regex.h>

#define buffer_size 4096
#define filename_size 128
#define options_count 8

void get_templates(int *templates_count, char templates[][filename_size], int *template_files_count, char template_files[][filename_size], int argc, char **argv);
void cut_from_argv(char option, int i, int j, int *count, char array[][filename_size], int argc, char **argv);
void get_options(int *options, int argc, char **argv);
void get_files(char **files, int *n, int argc, char **argv);
void file_processing(FILE *file, int files_count, char* filename, int* options, 
                    int templates_count, char templates[][filename_size],
                    int template_files_count, char template_files[][filename_size]);
char* pattern_searching(char *buffer, int templates_count, char templates[][filename_size],
                    int template_files_count, char template_files[][filename_size], int *options);
char* regular_searching(char *text, char *expr, int *options);
void line_by_line_output(char *buffer, char *filename, int line_number, int files_count, int *options);

int main(int argc, char **argv) {

    int template_files_count = 0, templates_count = 0;
    char template_files[argc][filename_size], templates[argc][filename_size];
    get_templates(&templates_count, templates, &template_files_count, template_files, argc, argv);

    int options[options_count];
    get_options(options, argc, argv);  // except -e and -f

    int files_count = 0;
    char *files[argc];
    get_files(files, &files_count, argc, argv);

    if (templates_count == 0 && template_files_count == 0) {
        fprintf(stderr, "s21_grep: No template\n");
        exit(1);
    }

    if (files_count == 0) {  // console mode
        file_processing(stdin, 0, 0, options, templates_count, templates, template_files_count, template_files);
    } else {  // file mode
        FILE *file = NULL;
        for (int i = 0; i < files_count; i++) {
            file = fopen(files[i], "r");
            if (file) {
                file_processing(file, files_count, files[i], options, templates_count, templates, template_files_count, template_files);
            } else {
                if (!options[6])  // suppress error messages about nonexistent or unreadable files
                    fprintf(stderr, "s21_grep: %s: No such file or directory\n", argv[i]);
            }
            fclose(file);
        }
    }

    return 0;
}

void file_processing(FILE *file, int files_count, char* filename, int* options, 
                    int templates_count, char templates[][filename_size],
                    int template_files_count, char template_files[][filename_size]) {

    char buffer[buffer_size]; int match;
    int line_number = 0, match_count = 0;
    char *match_part;
    while (fgets(buffer, buffer_size, file)) {
        line_number++;
        match_part = pattern_searching(buffer, templates_count, templates, template_files_count, template_files, options);
        if (match_part != NULL) {
            match = 1;
        } else {
            match = 0;
        }
        if ((match && !options[0]) || (!match && options[0])) {  // -v - invert match
            match_count++;
            if (!options[3] && !options[2])
                line_by_line_output(buffer, filename, line_number, files_count, options);
        }
    }
    if (options[3]) {  // -l - output matching files only
        if (match_count) {
            fprintf(stdout, "%s\n", filename);
        }
    } else if (options[2]) {  // -c - output count of matching lines only
        if (files_count == 1 || options[5]) {  // -h - suppress the prefixing of file names on output
            fprintf(stdout, "%d\n", match_count);
        } else {
            fprintf(stdout, "%s:%d\n", filename, match_count);
        }
    }
}

void line_by_line_output(char *buffer, char *filename, int line_number, int files_count, int *options) {

    if (!options[3] && !options[2]) {
        if (files_count == 1 || options[5]) {  // -h - suppress the prefixing of file names on output
            if (options[4]) {  // -n - precede each matching line with a line number
                fprintf(stdout, "%d:%s\n", line_number, buffer);
            } else {
                fprintf(stdout, "%s\n", buffer);
            }
        } else {
            if (options[4]) {  // -n - precede each matching line with a line number
                fprintf(stdout, "%s:%d:%s\n", filename, line_number, buffer);
            } else {
                fprintf(stdout, "%s:%s\n", filename, buffer);
            }
        }
    }
}

char* pattern_searching(char *buffer, int templates_count, char templates[][filename_size],
                    int template_files_count, char template_files[][filename_size], int *options) {

    char *result = NULL; char *temp;
    FILE *file; char template[filename_size];

    for (int i = 0; i < templates_count; i++) {
        result = regular_searching(buffer, templates[i], options);
        if (result != NULL)
            break;
    }

    if (result == NULL) {
        for (int i = 0; i < template_files_count; i++) {
            file = fopen(template_files[i], "r");
            while (fgets(template, filename_size, file)) {
                temp = strrchr(template, '\n');
                if (temp != NULL)
                    *temp = 0;
                result = regular_searching(buffer, template, options);
                if (result != NULL)
                    break;
            }
            fclose(file);
            if (result != NULL)
                break;
        }
    }
    return result;
}

char* regular_searching(char *text, char *expr, int *options) {

    char *result = NULL;
    char *temp;

    temp = strrchr(text, '\n');
    if (temp != NULL)
        *temp = 0;

    if (text[0] != 0) {
        regex_t regexpr;
        char message[filename_size];
        int error = 0;

        if (options[1]) {  // ignore uppercase vs. lowercase
            error = regcomp(&regexpr, expr, REG_EXTENDED|REG_ICASE);
        } else {
            error = regcomp(&regexpr, expr, REG_EXTENDED);
        }

        if (error != 0) {
            regerror(error, &regexpr, message, filename_size);
            printf("%s\n", message);
            exit(1);
        }

        if ((error = regexec(&regexpr, text, 0, NULL, 0)) == 0)
            result = text;

        else if ( error != REG_NOMATCH ) {
            regerror(error, &regexpr, message, filename_size);
            exit(1);
        }

        regfree(&regexpr);
    }

    return result;
}

void get_templates(int *templates_count, char templates[][filename_size], int *template_files_count, char template_files[][filename_size], int argc, char **argv) {

    int j;
    for (int i = 1; i < argc; i++) {  // templates extracting with -e and -f flags
        if (argv[i][0] == '-') {
            for (j = 1; argv[i][j] && (argv[i][j] != 'e' && argv[i][j] != 'f'); j++);
            if (argv[i][j] == 'e') {
                cut_from_argv('e', i, j, templates_count, templates, argc, argv);
            } else if (argv[i][j] == 'f') {
                cut_from_argv('f', i, j, template_files_count, template_files, argc, argv);
            }
        }
    }
    if (*templates_count == 0 && *template_files_count == 0) {  // template extracting if there's no -e or -f flags
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] != '-' && argv[i][0] != 0) {
                strcpy(templates[0], argv[i]);
                *templates_count += 1;
                argv[i][0] = 0;
                break;
            }
        }
    }
    FILE *file;
    for (int i = 0; i < *template_files_count; i++) {  // checks if all of the template files exist
        file = fopen(template_files[i], "r");
        if (file == NULL) {
            fprintf(stderr, "s21_grep: %s: No such file or directory\n", template_files[i]);
            exit(1);
        }
        fclose(file);
    }
}

void cut_from_argv(char option, int i, int j, int *count, char array[][filename_size], int argc, char **argv) {
    
    if (argv[i][j+1] == 0) {  // the template/filename is in the next argv
        if (i+1 >= argc) {  // there's no template/filename
            fprintf(stderr, "s21_grep: option requires an argument -- '%c'", option);
            exit(1);
        } else {
            strcpy(array[*count], argv[i+1]);
            *count += 1;
            argv[i][1] = 0;
            argv[i+1][0] = 0;
        }
    } else {  // the template/filename is in the current argv after the letter e/f
        strcpy(array[*count], argv[i]+j+1);
        *count += 1;
        argv[i][j] = 0;
    }
}

void get_options(int *options, int argc, char **argv) {
    for (int i = 0; i < options_count; i++)
        options[i] = 0;
    char temp;
    while ((temp = getopt(argc, argv, "viclnhsoef")) != -1) {
        switch(temp) {
            case 'v':
                options[0] = 1;
                break;
            case 'i':
                options[1] = 1;
                break;
            case 'c':
                options[2] = 1;
                break;
            case 'l':
                options[3] = 1;
                break;
            case 'n':
                options[4] = 1;
                break;
            case 'h':
                options[5] = 1;
                break;
            case 's':
                options[6] = 1;
                break;
            case 'o':
                options[7] = 1;
                break;
            case 'e':
                break;
            case 'f':
                break;
            default:
                fprintf(stdin, "s21_grep: invalid option -- '%c'", temp);
                exit(1);
        }
    }
}

void get_files(char **files, int* n, int argc, char **argv) {

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-' && argv[i][0] != 0) {
            files[*n] = argv[i];
            *n += 1;
        }
    }
}
