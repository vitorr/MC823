all:
	gcc -Wall main.c -o server.out -g

run:
	./server.out

data:
	gcc -Wall create_database.c -o create_database.out -g
	gcc -Wall check_database.c -o check_database.out -g
	./create_database.out

clean:
	-rm bookstore_database.bin
	-rm file*.log
	-rm *.out
