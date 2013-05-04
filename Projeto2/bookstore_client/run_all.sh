if [ $# -eq 0 ]; then
	times=5
else
	times=$1
fi

for i in `seq 1 $times`
do
	./client.out 0
	echo "1200000000001" | ./client.out 1
	echo "1200000000001" | ./client.out 2
	echo -e "1200000000001\n8000" | ./client.out 4 bookstore
	echo "1200000000001" | ./client.out 5
	./client.out 3
done
