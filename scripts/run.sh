make all CUSTOM="-D BRANCH_HISTORY_LENGTH=1"
./cbp ../traces/small/int_3_trace.gz > 1.txt

for i in {1,16}
do
    make all CUSTOM="-D BRANCH_HISTORY_LENGTH=$i"
    ./cbp ../traces/small/int_3_trace.gz > $i.txt
done