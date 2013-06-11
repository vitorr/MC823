all:
	gcc -Wall -lrt client.c -o client.out -g

run:
	./run.sh

clean:
	-rm *.out

down:
	-(cd ../logs && make down)
