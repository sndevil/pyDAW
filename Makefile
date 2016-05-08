## GNU makefile for Linux.
##
## Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
## Creation Date: Sun Feb  3 10:15:51 PST 2002
## Last Modified: Sun Feb  3 10:15:54 PST 2002
## Filename:      ...soundfile/Makefile
##
## Description: This Makefile can create the soundfile library or example 
##              programs which use the soundfile library in linux using 
##              gcc 2.7.2 or higher.
##
## To run this makefile, type (without quotes)
## (1)  "make library"    -- to compile sig library (into soundfile/lib)
## (2)  "make examples"   -- to compile example programs (into soundfile/bin)
##

# MAKE stores the name of the makefile program:
MAKE = make

INCDIR    = include
LIBDIR    = /usr/local/lib

OUT=bin/out
OBJ=obj/main.o
PREFLAGS    := -Wall -g -O6 -I$(INCDIR)

# Uncomment the following line for static linking (more portable executable)
# PREFLAGS += -static

POSTFLAGS := -L$(LIBDIR) -lsndfile

GCC=g++
FLG=-std=c++14

# commands which do not refer to actual files:
.PHONY: all clean lib library examples example

###########################################################################
#                                                                         #
#                                                                         #

all: gccDAW

gccDAW: $(OBJ)
	$(GCC) $(PREFLAGS) $^ -o $(OUT) $(POSTFLAGS) $(FLG)

obj/main.o: src/main.cc
	$(GCC) $(PREFLAGS) -c $^ -o $@ $(POSTFLAGS) $(FLG)
	

lib: library
library:
	$(MAKE) -f Makefile.library

clean:
	rm $(OUT)
	rm $(OBJ)
%: 
	@echo compiling file $@
	$(MAKE) -f Makefile.examples $@

#                                                                         #
#                                                                         #
###########################################################################



