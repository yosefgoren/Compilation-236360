TEST_NAME=$1
if [ -z $2 ]; then
	LEXER_EXE="../hw1.out"
else
	LEXER_EXE=$2
fi

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

if [ ! -f $LEXER_EXE ]; then
	printf "${RED}Error: lexer executable not found! ${NC}\n"
	exit 1
fi

$LEXER_EXE > $TEST_NAME.result < $TEST_NAME.in
diff $TEST_NAME.expected $TEST_NAME.result
if [ $? -eq 0 ]; then
	printf "$TEST_NAME: ${GREEN} SUCCESS ${NC}\n"
else
	printf "$TEST_NAME: ${RED} FAILURE ${NC}\n"
	printf "\t${BLUE}< expected but not found${NC}\n"
	printf "\t${BLUE}> found but not expected${NC}\n"
	exit 1
fi
