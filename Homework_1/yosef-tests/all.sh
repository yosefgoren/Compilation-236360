./check.sh pdf/example_test
for i in {1..11}
do 
	./check.sh old/t$i
done
for i in {1..2}
do
	./check.sh webc/t$i
done
for i in {1..16}
do
	./check.sh my/t$i
done
