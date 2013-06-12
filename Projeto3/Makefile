.PHONY: all server client data clean rmiregistry

all:
	javac -d ./bin ./src/*.java

rmiregistry:
	-killall java
	-pkill rmiregistry
	(cd bin && rmiregistry &)

server: rmiregistry
	(cd bin && java  -classpath . unicamp.mc823.Server &)
	@sleep 2

client:
	(cd bin && java  -classpath . unicamp.mc823.Client ${OP} $U)

get_ip:
	curl -s http://www.students.ic.unicamp.br/~ra091187/mc823/ip.txt > bin/ip.txt
	cat bin/ip.txt | sed "s/1/#/g" | sed "s/2/1/g" | sed "s/#/2/g" > bin/ip2.txt

set_ip:
	ifconfig | awk -F':' '/inet addr/&&!/127.0.0.1/{split($$2,_," ");print _[1]}' | sed "s/1/#/g" | sed "s/2/1/g" | sed "s/#/2/g" > bin/ip.txt
	scp bin/ip.txt ra091187@ssh.students.ic.unicamp.br:~/public_html/mc823/ip.txt

clean:
	-rm -rf bin
	-mkdir bin
	cp src/database_orig.txt bin/database.txt
	(cd logs && make clean)
