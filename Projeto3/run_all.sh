if [ $# -eq 0 ]; then
	times=1
else
	times=$1
fi

for i in `seq 1 $times`
do
	(cd bin && java  -classpath . unicamp.mc823.Client 0)
	echo "1200000000001" | (cd bin && java  -classpath . unicamp.mc823.Client 1)
	echo "1200000000001" | (cd bin && java  -classpath . unicamp.mc823.Client 2)
	echo -e "1200000000001\n8000" | (cd bin && java  -classpath . unicamp.mc823.Client 4 bookstore)
	echo "1200000000001" | (cd bin && java  -classpath . unicamp.mc823.Client 5)
	(cd bin && java  -classpath . unicamp.mc823.Client 3)
done
