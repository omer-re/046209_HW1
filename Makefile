# Makefile for the smash program
# FOR C++
CC = g++
CFLAGS = -g -Wall
CXXFLAGS= -std=c++11
CCLINK = $(CC)
OBJS = smash.o commands.o signals.o
RM = rm -f
# Creating the  executable
smash: $(OBJS)
	$(CCLINK) -o smash $(OBJS)
# Creating the object files
commands.o: commands.cpp commands.h
smash.o: smash.cpp commands.h
signals.o: signals.cpp signals.h
# Cleaning old files before new make
clean:
	rm -f $(PROG) $(OBJS) $(TARGET) *.o *~ "#"* core.*

