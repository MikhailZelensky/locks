make
touch myfile
./program myfile &
pid=$!
while [ ! -f myfile.lck ]; do
	sleep 0.1
done
echo 1 > myfile.lck
wait $pid
