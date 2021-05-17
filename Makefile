# Yonatan Gizachew
# 20170750
# Assignment 5
# Makefile

# Dependency rules for non-file targets
all: ish

clean:
	rm -f ish ish*.o parse_command*.o syntactic*.o dfa*.o dynarray*.o

# Depencency rules for file targets
ish: ish.o parse_command.o syntactic.o dfa.o dynarray.o
	gcc209 -g ish.o parse_command.o syntactic.o dfa.o dynarray.o -o ish

ish.o: ish.c parse_command.h syntactic.h dfa.h dynarray.h
	gcc209 -g -c ish.c
parse_command.o: parse_command.c syntactic.h dfa.h dynarray.h
	gcc209 -g -c parse_command.c
syntactic.o: syntactic.c dfa.h dynarray.h
	gcc209 -g -c syntactic.c

dfa.o: dfa.c dynarray.h
	gcc209 -g -c dfa.c

dynarray.o: dynarray.c
	gcc209 -g -c dynarray.c
