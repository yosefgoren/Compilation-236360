#!/usr/bin/env python3


import random
import subprocess
import difflib
import os
import argparse


class ColorText:

    BLUE = '\033[94m'
    MAGENTA = '\033[95m'
    YELLOW = '\033[93m'
    GREEN = '\033[92m'
    RED = '\033[91m'
    END = '\033[0m'

class Tester:

    TESTS_DIR = os.path.dirname(os.path.realpath(__file__))
    TEST_FILES_DIR = os.path.join(TESTS_DIR, "test_files")
    UTILS_DIR = os.path.join(TESTS_DIR, "utils")
    MAIN_DIR = os.path.join(TESTS_DIR, os.pardir)
    INPUT_FILE = os.path.join(TEST_FILES_DIR, "input")
    OUTPUT_FILE = os.path.join(TEST_FILES_DIR, "output")
    EXPECTED_FILE = os.path.join(TEST_FILES_DIR, "expected")
    DIFF_FILE = os.path.join(TEST_FILES_DIR, "diff")
    CONFIG_FILE = os.path.join(TESTS_DIR, "config")
    ERROR_MSG_FILE = os.path.join(UTILS_DIR, "error_msg.txt")
    BASIC_TOKENS_FILE = os.path.join(UTILS_DIR, "basic_tokens.txt")
    BINOP_TOKEN_FILE = os.path.join(UTILS_DIR, "binop_token.txt")
    RELOP_TOKEN_FILE = os.path.join(UTILS_DIR, "relop_token.txt")
    TOKEN_TYPES = ["BASIC", "BINOP", "RELOP", "COMMENT", "ID", "NUM", "STRING"]
    BASIC_TOKENS_DICT = {}
    BINOP_TOKEN_LIST = []
    RELOP_TOKEN_LIST = []
    PRINTABLE_CHARS = []
    HEX_CHARS = []
    SPECIAL_STRING_CHARS = []
    with open(CONFIG_FILE, 'r') as config_file:
        GENERATED_TOKENS = int(config_file.readline().strip('\n'))
        GENERATED_FILES = int(config_file.readline().strip('\n'))
        EXEC_FILE = config_file.readline().strip('\n')
        COMP_CMD = []
        COMP_CMD.append(config_file.readline().strip('\n').split())
        COMP_CMD.append(config_file.readline().strip('\n').split())
    # TODO: there are some serious issues with \r in csComp
    # WHITESPACES = [' ', '\t', '\n', '\r']
    WHITESPACES = [' ', '\t', '\n']
    # NEW_LINE = ['\n', '\r']
    NEW_LINE = ['\n']
    FAILED_TESTS_ERROR_MSG = """
Please check the diff files in the tests/test_files directory.
You can do so by running the tester with the following flag:
--show_diff <test_num>

For instance:
tests/tester.py --show_diff 8

In order to compile again and run a specific test without generating
any new input files run the tester with the following flags:
--compile --run_test <test_num>

For instance:
tests/tester.py --compile --run_test 4
"""
    RUN_TEST_CMD = [os.path.join(MAIN_DIR, EXEC_FILE)]


    @staticmethod
    def colored_print(str, color, new_line):
        if color == "RED":
            print(f"{ColorText.RED}{str}{ColorText.END}", end='', flush=True)
        elif color == "GREEN":
            print(f"{ColorText.GREEN}{str}{ColorText.END}", end='', flush=True)
        elif color == "YELLOW":
            print(f"{ColorText.YELLOW}{str}{ColorText.END}", end='', flush=True)
        elif color == "MAGENTA":
            print(f"{ColorText.MAGENTA}{str}{ColorText.END}", end='', flush=True)
        elif color == "DEFAULT":
            print(str, end='', flush=True)
        if new_line:
            print('', flush=True)

    @staticmethod
    def calc_printed_spaces(file_index):
        spaces = ' ' * (13 - len(str(file_index)))
        return spaces

    @staticmethod
    def get_args():
        parser = argparse.ArgumentParser()
        parser.add_argument("-c", "--compile",
                            action="store_true",
                            default=False,
                            help="compile source files before running tests")
        parser.add_argument("-r", "--run_test",
                            default="all",
                            help="run test on specific input file")
        parser.add_argument("-k", "--keep_files",
                            action="store_true",
                            default=False,
                            help="don't generate new input files")
        parser.add_argument("-d", "--dont_test",
                            action="store_true",
                            default=False,
                            help="don't run any tests")
        parser.add_argument("-s", "--show_diff",
                            default=None,
                            help="show diff file of specific test")
        return parser.parse_args()

    @classmethod
    def print_diff_file(cls, diff_file_num):
        diff_file_name = cls.DIFF_FILE + f"_{diff_file_num}.txt"
        if not os.path.isfile(diff_file_name):
            cls.colored_print("File does not exist!\n", "RED", new_line=True)
        else:
            with open(diff_file_name, 'r') as diff_file:
                cls.colored_print("\nDiff file location:", "YELLOW", new_line=True)
                cls.colored_print(diff_file_name, "DEFAULT", new_line=True)
                cls.colored_print("\nPrinting diff file...", "YELLOW", new_line=True)
                cls.colored_print(diff_file.read(), "DEFAULT", new_line=True)
        exit()

    @classmethod
    def clean_env(cls):
        current_dir = os.getcwd()
        os.chdir(cls.TEST_FILES_DIR)
        for file_to_delete in os.listdir(cls.TEST_FILES_DIR):
            os.remove(file_to_delete)
        os.chdir(current_dir)

    @classmethod
    def generate_basic_tokens_dict(cls):
        with open(cls.BASIC_TOKENS_FILE) as basic_tokens_file:
            line_number = 0
            for next_line in basic_tokens_file:
                next_line = next_line.strip("\n").split()
                token_name = next_line[0]
                token_value = next_line[1]
                cls.BASIC_TOKENS_DICT[line_number] = [token_name, token_value]
                line_number += 1

    @classmethod
    def generate_binop_token_list(cls):
        with open(cls.BINOP_TOKEN_FILE, 'r') as binop_token_file:
            for next_line in binop_token_file:
                cls.BINOP_TOKEN_LIST.append(next_line.strip('\n'))

    @classmethod
    def generate_relop_token_list(cls):
        with open(cls.RELOP_TOKEN_FILE, 'r') as relop_token_file:
            for next_line in relop_token_file:
                cls.RELOP_TOKEN_LIST.append(next_line.strip('\n'))

    @classmethod
    def generate_printable_chars_list(cls):
        low_limit = int('20', 16)
        high_limit = int('7E', 16)
        for printable_char in range(low_limit, high_limit + 1):
            cls.PRINTABLE_CHARS.append(chr(printable_char))
        cls.PRINTABLE_CHARS.remove("\\")
        cls.PRINTABLE_CHARS.remove("\"")

    @classmethod
    def generate_hex_chars_list(cls):
        for right_digit in ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F']:
            for left_digit in ['0', '1', '2', '3', '4', '5', '6', '7']:
                cls.HEX_CHARS.append("\\x" + left_digit + right_digit)

    @classmethod
    def generate_special_string_chars_list(cls):
        cls.SPECIAL_STRING_CHARS = ["\\n", "\\r", "\\t", "\\\\", "\\0", "\\\""]

    @classmethod
    def set_vars(cls):
        cls.generate_basic_tokens_dict()
        cls.generate_binop_token_list()
        cls.generate_relop_token_list()
        cls.generate_printable_chars_list()
        cls.generate_hex_chars_list()
        cls.generate_special_string_chars_list()
        cls.colored_print("     DONE", "GREEN", new_line=True)

    @classmethod
    def prepare_env(cls, args):
        cls.colored_print("Preparing environment...", "MAGENTA", new_line=False)
        if (not args.keep_files) and (args.run_test == "all"):
            Tester.clean_env()
        Tester.set_vars()

    @classmethod
    def get_random_basic_token(cls):
        random_index = random.randint(0, len(cls.BASIC_TOKENS_DICT) - 1)
        random_dict_item = cls.BASIC_TOKENS_DICT.get(random_index)
        return random_dict_item[0], random_dict_item[1], random_dict_item[1]

    @classmethod
    def get_random_binop_token(cls):
        binop_token_value = random.choice(cls.BINOP_TOKEN_LIST)
        return "BINOP", binop_token_value, binop_token_value

    @classmethod
    def get_random_relop_token(cls):
        relop_token_value = random.choice(cls.RELOP_TOKEN_LIST)
        return "RELOP", relop_token_value, relop_token_value

    @classmethod
    def get_random_comment_token(cls):
        length = random.randint(0, 100)
        comment_content = "//"
        low_limit = int('00', 16)
        high_limit = int('7F', 16)
        for _ in range(length):
            while True:
                random_char = chr(random.randint(low_limit, high_limit))
                if random_char != '\n' and random_char != '\r':
                    break
            comment_content += random_char
        comment_content += random.choice(cls.NEW_LINE)
        return "COMMENT", comment_content, "//"

    @classmethod
    def is_id_legal(cls, id_value):
        for i in range(len(cls.BASIC_TOKENS_DICT)):
            current_value = (cls.BASIC_TOKENS_DICT[i])[1]
            if current_value == id_value:
                return False
        return True

    @classmethod
    def get_random_id_token(cls):
        while True:
            big_letter_low_limit = int('41', 16)
            big_letter_high_limit = int('5A', 16)
            little_letter_low_limit = int('61', 16)
            little_letter_high_limit = int('7A', 16)
            digit_low_limit = int('30', 16)
            digit_high_limit = int('39', 16)
            first_letter_type = random.choice(["BIG", "LITTLE"])
            if first_letter_type == "BIG":
                first_letter = chr(random.randint(big_letter_low_limit, big_letter_high_limit))
            else: # first_letter_type == "LITTLE"
                first_letter = chr(random.randint(little_letter_low_limit, little_letter_high_limit))
            id_value = first_letter
            length = random.randint(0, 100)
            for _ in range(length):
                char_type = random.choice(["BIG_LETTER", "LITTLE_LETTER", "DIGIT"])
                if char_type == "BIG_LETTER":
                    id_value += chr(random.randint(big_letter_low_limit, big_letter_high_limit))
                elif char_type == "LITTLE_LETTER":
                    id_value += chr(random.randint(little_letter_low_limit, little_letter_high_limit))
                else: # char_type == "DIGIT"
                    id_value += chr(random.randint(digit_low_limit, digit_high_limit))
            if cls.is_id_legal(id_value):
                break
        return "ID", id_value, id_value

    @classmethod
    def get_random_num_token(cls):
        digits = [*range(0,10)]
        first_digit = random.choice(digits)
        num_value = str(first_digit)
        if first_digit != 0:
            length = random.randint(0, 10)
            for _ in range(length):
                num_value += str(random.choice(digits))
        return "NUM", num_value, num_value

    @classmethod
    def get_parsed_char(cls, c):
        parsed_char = c.replace("\\n", "\n")
        if parsed_char != c:
            return parsed_char
        parsed_char = c.replace("\\r", "\r")
        if parsed_char != c:
            return parsed_char
        parsed_char = c.replace("\\t", "\t")
        if parsed_char != c:
            return parsed_char
        parsed_char = c.replace("\\0", "\0")
        if parsed_char != c:
            return parsed_char
        parsed_char = c.replace("\\\"", "\"")
        if parsed_char != c:
            return parsed_char
        parsed_char = c.replace("\\\\", "\\")
        if parsed_char != c:
            return parsed_char
        if c.startswith("\\x"):
            parsed_char = chr(int(c[2:4], 16))
        return parsed_char

    @classmethod
    def get_random_string_char(cls):
        char_type = random.choice(["PRINTABLE", "HEX", "SPECIAL"])
        if char_type == "PRINTABLE":
            random_char = random.choice(cls.PRINTABLE_CHARS)
        elif char_type == "HEX":
            random_char = random.choice(cls.HEX_CHARS)
        else: # char_type == "SPECIAL"
            random_char = random.choice(cls.SPECIAL_STRING_CHARS)
        return random_char, cls.get_parsed_char(random_char)

    @classmethod
    def get_random_string_token(cls):
        length = random.randint(0, 100)
        string_content_input = '"'
        string_content_expected = ''
        for _ in range(length):
            next_char_input, next_char_expected = cls.get_random_string_char()
            string_content_input += next_char_input
            string_content_expected += next_char_expected
        string_content_input += '"'
        return "STRING", string_content_input, string_content_expected

    @classmethod
    def get_random_token(cls):
        token_type = random.choice(cls.TOKEN_TYPES)
        if token_type == "BASIC":
            return cls.get_random_basic_token()
        elif token_type == "BINOP":
            return cls.get_random_binop_token()
        elif token_type == "RELOP":
            return cls.get_random_relop_token()
        elif token_type == "COMMENT":
            return cls.get_random_comment_token()
        elif token_type == "ID":
            return cls.get_random_id_token()
        elif token_type == "NUM":
            return cls.get_random_num_token()
        elif token_type == "STRING":
            return cls.get_random_string_token()
        else:
            cls.colored_print("\nError: Unkown token type!\n", "RED", new_line=True)
            exit()

    @classmethod
    def get_random_whitespace(cls):
        random_index = random.randint(0, len(cls.WHITESPACES) - 1)
        random_whitespace = cls.WHITESPACES[random_index]
        return random_whitespace

    @classmethod
    def generate_single_file(cls, file_index):
        input_file_name = cls.INPUT_FILE + '_' + str(file_index) + ".txt"
        expected_file_name = cls.EXPECTED_FILE + '_' + str(file_index) + ".txt"
        with open(input_file_name, 'w') as input_file:
            with open(expected_file_name, 'w') as expected_file:
                current_line_number = 1
                for _ in range(cls.GENERATED_TOKENS):
                    whitespace = cls.get_random_whitespace()
                    token_name, token_value_input, token_value_expected = cls.get_random_token()
                    expected_line = str(current_line_number) + " "
                    expected_line += token_name + " "
                    expected_line += token_value_expected + "\n"
                    expected_file.write(expected_line)
                    input_file.write(token_value_input)
                    input_file.write(whitespace)
                    if token_name == "COMMENT":
                        current_line_number += 1
                    if whitespace in cls.NEW_LINE:
                        current_line_number += 1

    @classmethod
    def generate_files(cls):
        cls.colored_print("Generating files...", "MAGENTA", new_line=False)
        for i in range(cls.GENERATED_FILES):
            cls.generate_single_file(i)
        cls.colored_print("          DONE", "GREEN", new_line=True)

    @classmethod
    def compile(cls):
        cls.colored_print("Running flex command...", "MAGENTA", new_line=False)
        completed_proc = subprocess.run(cls.COMP_CMD[0],
                                        stdout=subprocess.DEVNULL,
                                        stderr=subprocess.DEVNULL)
        if completed_proc.returncode == 0:
            cls.colored_print("      PASSED", "GREEN", new_line=True)
        else:
            cls.colored_print("      FAILED", "RED", new_line=True)
            cls.colored_print("Printing errors:", "YELLOW", new_line=True)
            subprocess.run(cls.COMP_CMD[0])
            exit()
        cls.colored_print("Running g++ command...", "MAGENTA", new_line=False)
        completed_proc= subprocess.run(cls.COMP_CMD[1],
                                        stdout=subprocess.DEVNULL,
                                        stderr=subprocess.DEVNULL)
        if completed_proc.returncode == 0:
            cls.colored_print("       PASSED", "GREEN", new_line=True)
        else:
            cls.colored_print("       FAILED", "RED", new_line=True)
            cls.colored_print("Printing errors:", "YELLOW", new_line=True)
            subprocess.run(cls.COMP_CMD[1])
            exit()

    @classmethod
    def run_single_test(cls, input_file_name, output_file_name):
        with open(input_file_name, 'r') as input_file:
            with open(output_file_name, 'w') as output_file:
                subprocess.run(cls.RUN_TEST_CMD,
                                stdin=input_file,
                                stdout=output_file)

    @classmethod
    def check_diff_lines(cls, diff_file_name, diff_lines, file_index):
        spaces = cls.calc_printed_spaces(file_index)
        if len(diff_lines) == 0:
            cls.colored_print(f"{spaces}PASSED", "GREEN", new_line=True)
            return False
        else:
            cls.colored_print(f"{spaces}FAILED", "RED", new_line=True)
            with open(diff_file_name, 'w') as diff_file:
                for line in diff_lines:
                    diff_file.write(line + '\n')
            return True

    @classmethod
    def check_diff_single_file(cls, output_file_name, expected_file_name,
                                diff_file_name, file_index):
        diff_lines = []
        with open(output_file_name, 'r') as output_file:
            with open(expected_file_name, 'r') as expected_file:
                output_str = output_file.read().strip().splitlines()
                expected_str = expected_file.read().strip().splitlines()
                for line in difflib.unified_diff(output_str,
                                                    expected_str,
                                                    fromfile=output_file_name,
                                                    tofile=expected_file_name,
                                                    lineterm=''):
                    diff_lines.append(line)
        is_test_failed = cls.check_diff_lines(diff_file_name, diff_lines, file_index)
        return is_test_failed

    @classmethod
    def run_tests(cls, test_to_run):
        failed_tests_counter = 0
        for file_index in range(cls.GENERATED_FILES):
            if test_to_run != "all" and test_to_run != str(file_index):
                continue
            input_file_name = cls.INPUT_FILE + '_' + str(file_index) + ".txt"
            output_file_name = cls.OUTPUT_FILE + '_' + str(file_index) + ".txt"
            expected_file_name = cls.EXPECTED_FILE + '_' + str(file_index) + ".txt"
            diff_file_name = cls.DIFF_FILE + '_' + str(file_index) + ".txt"
            cls.colored_print(f"Running test {file_index}...", "MAGENTA", new_line=False)
            cls.run_single_test(input_file_name,
                                output_file_name)
            is_test_failed = cls.check_diff_single_file(output_file_name,
                                                        expected_file_name,
                                                        diff_file_name,
                                                        file_index)
            if is_test_failed:
                failed_tests_counter += 1
        if test_to_run == "all":
            if failed_tests_counter == 0:
                cls.colored_print("\nAll tests passed\n", "GREEN", new_line=True)
            else:
                cls.colored_print(f"\n{failed_tests_counter} tests failed!", "RED", new_line=True)
                cls.colored_print(cls.FAILED_TESTS_ERROR_MSG, "YELLOW", new_line=True)


def main():
    args = Tester.get_args()
    if args.show_diff:
        Tester.print_diff_file(args.show_diff)
    Tester.prepare_env(args)
    if (not args.keep_files) and (args.run_test == "all"):
        Tester.generate_files()
    if args.compile:
        Tester.compile()
    if not args.dont_test:
        Tester.run_tests(args.run_test)


if __name__ == '__main__':
    main()
