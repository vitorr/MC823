if [ $# -eq 0 ]; then
	times=5
else
	times=$1
fi

for i in `seq 1 $times`
do
	while true
	do
		./client.out 0 2> .tmp
		if ! grep -q "getaddrinfo: Name or service not known" .tmp
		then
			break
		fi
	done
	cat .tmp
	
	while true
	do
		echo "1200000000001" | ./client.out 1 2> .tmp
		if ! grep -q "getaddrinfo: Name or service not known" .tmp
		then
			break
		fi
	done
	cat .tmp
	
	while true
	do
		echo "1200000000001" | ./client.out 2 2> .tmp
		if ! grep -q "getaddrinfo: Name or service not known" .tmp
		then
			break
		fi
	done
	cat .tmp
	
	while true
	do
		echo -e "1200000000001\n8000" | ./client.out 4 bookstore 2> .tmp
		if ! grep -q "getaddrinfo: Name or service not known" .tmp
		then
			break
		fi
	done
	cat .tmp
	
	while true
	do
		echo "1200000000001" | ./client.out 5 2> .tmp
		if ! grep -q "getaddrinfo: Name or service not known" .tmp
		then
			break
		fi
	done
	cat .tmp
	
	while true
	do
		./client.out 3 2> .tmp
		if ! grep -q "getaddrinfo: Name or service not known" .tmp
		then
			break
		fi
	done
	cat .tmp
	rm .tmp
done
