.PHONY: all clean
SRC=scanner.lex parser.ypp output.cpp output.hpp
TAR_NAME=211515606-317580900

all: clean
	flex scanner.lex
	bison -Wcounterexamples -d parser.ypp
	g++ -std=c++17 -o hw2 *.c *.cpp
clean:
	rm -f lex.yy.c
	rm -f parser.tab.*pp
	rm -f hw2
tar:
	zip ${TAR_NAME} ${SRC}
