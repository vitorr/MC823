all:
	python graphs.py

up:
	scp server_op0.log ra091187@ssh.students.ic.unicamp.br:~/public_html/mc823/server_op0.log
	scp server_op1.log ra091187@ssh.students.ic.unicamp.br:~/public_html/mc823/server_op1.log
	scp server_op2.log ra091187@ssh.students.ic.unicamp.br:~/public_html/mc823/server_op2.log
	scp server_op3.log ra091187@ssh.students.ic.unicamp.br:~/public_html/mc823/server_op3.log
	scp server_op4.log ra091187@ssh.students.ic.unicamp.br:~/public_html/mc823/server_op4.log
	scp server_op5.log ra091187@ssh.students.ic.unicamp.br:~/public_html/mc823/server_op5.log

down:
	wget www.students.ic.unicamp.br/~ra091187/mc823/server_op0.log
	wget www.students.ic.unicamp.br/~ra091187/mc823/server_op1.log
	wget www.students.ic.unicamp.br/~ra091187/mc823/server_op2.log
	wget www.students.ic.unicamp.br/~ra091187/mc823/server_op3.log
	wget www.students.ic.unicamp.br/~ra091187/mc823/server_op4.log
	wget www.students.ic.unicamp.br/~ra091187/mc823/server_op5.log

clean:
	-rm *.log
