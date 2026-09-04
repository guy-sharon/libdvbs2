CC      ?= cc
AR      ?= ar
CFLAGS  ?= -std=c99 -Wall -Wextra -Wpedantic -O2
CPPFLAGS = -Iinclude

LIB       = libdvbs2.a
LIB_OBJ   = src/dvbs2.o
EXAMPLE   = examples/dvbs2_version
TEST      = tests/dvbs2_test_version

.PHONY: all test clean

all: $(LIB) $(EXAMPLE)

$(LIB): $(LIB_OBJ)
	$(AR) rcs $@ $^

$(EXAMPLE): examples/version.c $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LIB)

$(TEST): tests/test_version.c $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LIB)

test: $(TEST)
	./$(TEST)

clean:
	$(RM) $(LIB) $(LIB_OBJ) $(EXAMPLE) $(TEST)
