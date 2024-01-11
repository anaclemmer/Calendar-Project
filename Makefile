CC = gcc
CFLAGS = -ansi -Wall -g -O0 -Wwrite-strings -Wshadow -pedantic-errors \
		 -fstack-protector-all -Wextra
PROGS = student_tests public01 public02 public03 public04 public05 public06
OBJECTS = calendar.o my_memory_checker_216.o student_tests.o

.PHONY: all clean

all: $(PROGS) $(OBJECTS)

clean:
	@echo "deleting all object files and executables"
	@rm -f *.o $(PROGS) $(OBJECTS) *.tmp

public01: public01.o calendar.o my_memory_checker_216.o
	$(CC) public01.o calendar.o my_memory_checker_216.o -o public01
public01.o: public01.c calendar.h my_memory_checker_216.h event.h 
	$(CC) $(CFLAGS) -c public01.c
public02: public02.o calendar.o my_memory_checker_216.o
	$(CC) public02.o calendar.o my_memory_checker_216.o -o public02
public02.o: public02.c calendar.h my_memory_checker_216.h event.h 
	$(CC) $(CFLAGS) -c public02.c
public03: public03.o calendar.o my_memory_checker_216.o
	$(CC) public03.o calendar.o my_memory_checker_216.o -o public03
public03.o: public03.c calendar.h my_memory_checker_216.h event.h 
	$(CC) $(CFLAGS) -c public03.c
public04: public04.o calendar.o my_memory_checker_216.o
	$(CC) public04.o calendar.o my_memory_checker_216.o -o public04
public04.o: public04.c calendar.h my_memory_checker_216.h event.h 
	$(CC) $(CFLAGS) -c public04.c
public05: public05.o calendar.o my_memory_checker_216.o
	$(CC) public05.o calendar.o my_memory_checker_216.o -o public05
public05.o: public05.c calendar.h my_memory_checker_216.h event.h 
	$(CC) $(CFLAGS) -c public05.c
public06: public06.o calendar.o my_memory_checker_216.o
	$(CC) public06.o calendar.o my_memory_checker_216.o -o public06
public06.o: public06.c calendar.h my_memory_checker_216.h event.h 
	$(CC) $(CFLAGS) -c public06.c
student_tests: student_tests.o calendar.o my_memory_checker_216.o
	$(CC) student_tests.o calendar.o my_memory_checker_216.o -o student_tests
student_tests.o: student_tests.c calendar.h event.h my_memory_checker_216.h
	$(CC) $(CFLAGS) -c student_tests.c
calendar.o: calendar.c calendar.h event.h
	$(CC) $(CFLAGS) -c calendar.c
my_memory_checker_216.o: my_memory_checker_216.c my_memory_checker_216.h
	$(CC) $(CFLAGS) -c my_memory_checker_216.c