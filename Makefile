CC      ?= cc
AR      ?= ar
CFLAGS  ?= -std=c99 -Wall -Wextra -Wpedantic -O2
CPPFLAGS = -Iinclude

LIB      = libdvbs2.a
LIB_OBJ  = src/dvbs2.o
EXAMPLE  = examples/dvbs2_version
TEST_SRC := $(shell find tests -type f -name test.c | sort)
TEST_BIN := $(patsubst %.c,%,$(TEST_SRC))

.PHONY: all test clean $(TEST_BIN)

all: $(LIB) $(EXAMPLE)

$(LIB): $(LIB_OBJ)
	$(AR) rcs $@ $^

$(EXAMPLE): examples/version.c $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LIB)

$(TEST_BIN): $(LIB)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(dir $@)*.c $(LIB)
	./$@

test: $(TEST_BIN)
	@$(MAKE) --no-print-directory clean

clean:
	@$(RM) $(LIB) $(LIB_OBJ) $(EXAMPLE) $(TEST_BIN)
