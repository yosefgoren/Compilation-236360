./check.sh pdf/example_test
for i in {1..2}
do 
	./check.sh prov/t$i
done
for i in {1..74}
do 
	./check.sh old/t$i
done

