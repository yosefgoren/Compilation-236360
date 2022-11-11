ID1=211515606
ID2=317580900

all: clean
	flex scanner.lex
	bison -d parser.ypp -Wcounterexamples
	g++ -std=c++11 -o hw3 *.c *.cpp
clean:
	rm -f lex.yy.c
	rm -f parser.tab.*pp
	rm -f hw3
.PHONY: all clean

tar: 
	zip ${ID1}-${ID2} scanner.lex parser.ypp Symtab.cpp Symtab.hpp AuxTypes.cpp AuxTypes.hpp hw3_output.cpp hw3_output.hpp 