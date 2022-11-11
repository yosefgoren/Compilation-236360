TEST_NAME=$1
if [ -z $2 ]; then
	EXE="../hw2"
else
	EXE=$2
fi

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

if [ ! -f $EXE ]; then
	printf "${RED}Error: lexer executable not found! ${NC}\n"
	exit 1
fi

$EXE > $TEST_NAME.res < $TEST_NAME.in
diff $TEST_NAME.out $TEST_NAME.res
if [ $? -eq 0 ]; then
	printf "$TEST_NAME: ${GREEN} SUCCESS ${NC}\n"
else
	printf "$TEST_NAME: ${RED} FAILURE ${NC}\n"
	printf "\t${BLUE}< expected but not found${NC}\n"
	printf "\t${BLUE}> found but not expected${NC}\n"
	exit 1
fi
