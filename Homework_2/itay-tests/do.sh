./hw2tester 10 50 50 0
diff t0.out t0.expected > tmp0
cat tmp0 | grep "<" > tmp1
cat tmp0 | grep ">" > tmp2
head -n 5 tmp0
echo "..."
tail -n 5 tmp1
echo "..."
tail -n 5 tmp2
rm tmp*