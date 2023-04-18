make
touch myfile

for i in {1..10}; do
    success_count=0
    while true; do
        ./program myfile
        if [ $? = 0 ]; then
            success_count=$(($success_count + 1))
        fi
        echo "Process ${i}      success ${success_count}" > stat${i}
    done &
    pids[$i]=$!
done

sleep 300

for pid in ${pids[@]}; do
	kill $pid
done

for stat in stat*; do
    if [[ -f "$stat" ]]; then
        cat $stat >> stats_file
        rm $stat
	fi
done
