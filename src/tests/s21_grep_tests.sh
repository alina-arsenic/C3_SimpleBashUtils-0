rm -f ./tests_log

success=0
fail=0

# "-o"

for options in "" "-s" "-c" "-v" "-h" "-n" "-l" "-i" \
"-vc" "-vl" "-vn" "-vh" "-cl" "-cn" "-ch" "-ln" "-lh" "-nh" "-sc" "-sv" "-sh" "-sn" "-sl" "-si" "-ic" "-iv" "-ih" "-in" "-il"
#"-cvh" "-cvn" "-cvl" "-chn" "-chl" "-cnl" "-vhn" "-vhl" "-hnl" "-svc" "-svl" "-svn" "-svh" "-scl" "-scn" "-sch" "-sln" "-slh" "-snh" \
#"-icv" "-ich" "-icn" "-icl" "-ivh" "-ivn" "-ivl" "-ihn" "-ihl" "-sic" "-siv" "-sih" "-sin" "-sil" \
#"-cvhn" "-cvhl" "-cvnl" "-chnl" "-vhnl" "-icvh" "-icvn" "-icvl" "-ivhn" "-ivhl" "-ihnl" "-sicn" "-sicl" "-sivh" "-sicv" "-sich" \
#"-scvh" "-scvn" "-scvl" "-schn" "-schl" "-scnl" "-svhn" "-svhl" "-shnl" "-sivn" "-sivl" "-sihn" "-sihl" \
#"-scvhn" "-scvhl" "-scvnl" "-schnl" "-svhnl" "-sivhn" "-sivhl" "-sihnl" \
#"-sicvh" "-sicvn" "-sicvl" "-ivhnl" "-ichnl" "-icvnl" "-icvhl" "-icvhn" \
#"-sivhnl" "-sichnl" "-sicvnl" "-sicvhl" "-sicvhn"
do
    for template in "A" "a" "-e o" "-e o -e a" \
    "-f ../tests/patterns" "-f ../tests/patterns -f ../tests/patterns2" "-e o -e a -f ../tests/patterns -f ../tests/patterns2" "-f aboba" \
    "-ea" "-eae -e b" "-f../tests/patterns" "-faf"
    do
        for number in 1 2 3 4 5 6 7 8 9 10 11 "*"
        do
            grep $options $template ../tests/samples/text_$number > temp_grep 2>> temp_log
            ./s21_grep $options $template ../tests/samples/text_$number > temp_s21_grep 2>> temp_log
            diff temp_grep temp_s21_grep > temp_result
            if [ -s temp_result ]; then
                echo "./s21_grep $options $template ../tests/samples/text_$number FAIL" >> ./tests_log; fail=$(( $fail + 1 ))
            else
                echo "./s21_grep $options $template ../tests/samples/text_$number SUCCESS" >> ./tests_log; success=$(( $success + 1 ))
            fi
        done
    done
done

echo "Tests are done!"
echo "Successfully: $success"
echo "Failed: $fail"
echo "See details in tests_log"

rm -f temp*
