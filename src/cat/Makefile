CFLAGS=-Wall -Werror -Wextra
.PHONY: s21_cat

all: s21_cat s21_cat_tests

s21_cat:
	gcc $(CFLAGS) s21_cat.c -o s21_cat

s21_cat_tests:
	../tests/s21_cat_tests.sh

clean:
	rm -f ./s21_cat
