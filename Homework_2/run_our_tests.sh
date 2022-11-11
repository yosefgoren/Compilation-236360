# Compile
make clean
make all
echo

# Run staff tests
TEST_NUMBER=74

for i in $(seq 1 $TEST_NUMBER)
do
  echo "Running test "$i
  ./hw2 < our_tests/t"$i".in >& results/t"$i".myout
  diff our_tests/t"$i".out results/t"$i".myout
done
./hw2 < t1.in >& t1.myout
./hw2 < t2.in >& t2.myout
diff t1.out t1.myout
diff t2.out t2.myout