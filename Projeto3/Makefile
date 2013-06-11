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
	(cd bin && java  -classpath . unicamp.mc823.Client ${OP})

database:
	java  -classpath ./bin unicamp.mc823.Database

clean:
	-rm -rf bin/unicamp
	(cd src && cp database_orig.txt database.txt)
