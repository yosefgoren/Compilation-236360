hw1.out: lex.o hw1.o
	g++ $^ -o $@

lex.o: lex.yy.c
	g++ -o $@ -c $^

lex.yy.c: scanner.lex
	flex $^

hw1.o: hw1.cpp
	g++ -o $@ -c $^

reqcomp:
	flex scanner.lex
	g++ --std=c++17 lex.yy.c hw1.cpp -o hw1.out

selfcheck: tar
	./selfcheck.sh tar.zip

tar: hw1.cpp scanner.lex tokens.hpp
	zip -o $@ $^

clean:
	rm -rf *.o *.out lex.yy.c *.result selfcheck_tmp tar.zip