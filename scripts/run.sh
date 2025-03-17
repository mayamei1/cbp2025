n=2
for i in $(seq 1 $n)
do
    make all CUSTOM_FLAGS="-D BRANCH_HISTORY_LENGTH=$i"
    mv cbp run/cbp$i
done

for i in $(seq 1 $n)
do
    ./run/cbp$i /mnt/designkits/cbp2025/int/int_3_trace.gz > run/$i.txt &
done

wait $(jobs -p)
echo Done