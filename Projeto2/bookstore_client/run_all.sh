if [ $# -eq 0 ]; then
	times=5
else
	times=$1
fi

for i in `seq 1 $times`
do
	echo op0
	./client.out 0
	echo
	echo op1
	echo "1200000000001" | ./client.out 1 &
	echo
	echo op2
	echo "1200000000001" | ./client.out 2 &
	echo
	echo op3
	./client.out 3 &
	echo
	echo op4
	echo -e "1200000000001\n8000" | ./client.out 4 bookstore &
	echo
	echo op5
	echo "1200000000001" | ./client.out 5 &
done

