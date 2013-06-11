all:
	python graphs.py

up:
	scp server_op* ra091187@ssh.students.ic.unicamp.br:~/public_html/mc823/

down:
	for i in `seq 0 5` ; do \
		curl -s http://www.students.ic.unicamp.br/~ra091187/mc823/server_op$$i.log > server_op$$i.log ; \
	done

clean:
	-rm *.log
