CMP = gcc
FLAGS = -g -Wall -Wextra -lm

.PHONY: clean all

all: test_NFA test_DFA test_parser parser

%.o: %.c
	$(CMP) $(FLAGS) -c -o $@ $<

NFA.c: NFA.h
NFA_regex.c: NFA.h
NFA_test.c: NFA.h
NFA_print.c: NFA_print.h
conversion.c: conversion.h
parser.c: NFA.h parser.h
parser_test.c: NFA.h parser.h

test_NFA: NFA_test.o NFA.o NFA_regex.o NFA_print.o
	$(CMP) $(FLAGS) -o $@ $^
test_DFA: DFA_test.o DFA.o DFA_print.o
	$(CMP) $(FLAGS) -o $@ $^
test_parser: parser_test.o NFA.o NFA_regex.o NFA_print.o parser.o
	$(CMP) $(FLAGS) -o $@ $^
parser: main.o parser.o NFA.o NFA_regex.o NFA_print.o conversion.o DFA_print.o DFA.o
	$(CMP) $(FLAGS) -o $@ $^
clean:
	rm -rf *.o test_NFA test_DFA test_parser parser
