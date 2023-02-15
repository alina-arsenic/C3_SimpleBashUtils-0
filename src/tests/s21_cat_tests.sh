rm -f ./tests_log

success=0
fail=0

for options in "" "-b" "-e" "-n" "-s" "-t" "-be" "-bn" "-bs" "-bt" "-en" "-es" "-et" "-ns" "-nt" "-st" \
"-ben" "-bes" "-bet" "-bns" "-bnt" "-bst" "-ens" "-ent" "-est" "-nst" "-enst" "-bnst" "-best" "-bent" "-bens" "-benst" \
"-e -e" "-bn -st" "-b -e -n -s -t" "-bbbbenst" "-ajiods" "-e -y" # "-E" "-T" "--number-nonblank" "--number" "--squeeze-blank" "--do-shit"
do
    for number in 1 2 3 4 5 6 7 8 9 "*"
    do
        cat $options ../tests/samples/text_$number > temp_cat 2>> temp_log
        ./s21_cat $options ../tests/samples/text_$number > temp_s21_cat 2>> temp_log
        diff temp_cat temp_s21_cat > temp_result
        if [ -s temp_result ]; then
            echo "./s21_cat $options ../tests/samples/text_$number FAIL" >> ./tests_log; fail=$(( $fail + 1 ))
        else
            echo "./s21_cat $options ../tests/samples/text_$number SUCCESS" >> ./tests_log; success=$(( $success + 1 ))
        fi
    done
done

echo "Tests are done!"
echo "Successfully: $success"
echo "Failed: $fail"
echo "See details in tests_log"

rm -f temp*
