CFLAGS=-Wall -Werror -Wextra
.PHONY: s21_cat s21_grep

all: s21_grep s21_cat

s21_cat:
	gcc $(CFLAGS) cat/s21_cat.c -o cat/s21_cat

s21_grep:
	gcc $(CFLAGS) grep/s21_grep.c -o grep/s21_grep

clean:
	rm -f ./cat/s21_cat ./grep/s21_grep
