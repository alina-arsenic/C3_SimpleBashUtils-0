CFLAGS=-Wall -Werror -Wextra
.PHONY: s21_grep

all: s21_grep s21_grep_tests

s21_grep:
	gcc $(CFLAGS) s21_grep.c -o s21_grep

s21_grep_tests:
	../tests/s21_grep_tests.sh

clean:
	rm -f ./s21_grep
