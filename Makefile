ifeq ($(origin CC), default)
CC := $(shell (command -v cc >/dev/null 2>&1 && echo cc) || (where cc >/dev/null 2>&1 && echo cc) || echo gcc)
endif
AR      ?= ar
CFLAGS  ?= -std=c99 -Wall -Werror -Wextra -Wpedantic -O2
CPPFLAGS += -Iinclude

EXE :=
ifeq ($(OS),Windows_NT)
EXE := .exe
endif

LIB      = libdvbs2.a
SRC      := $(shell find src -type f -name '*.c' -print | sort)
LIB_OBJ  := $(SRC:.c=.o)
EXAMPLE  = examples/dvbs2_version$(EXE)
TEST_SRC := $(shell find tests -maxdepth 1 -type f -name '*_test.c' -print | sort)
TEST_SUPPORT := $(shell find tests -type f -name '*.c' ! -name '*_test.c' -print | sort)
TEST_BIN := $(patsubst %.c,%$(EXE),$(TEST_SRC))

.PHONY: all test clean $(TEST_BIN)

all: $(LIB) $(EXAMPLE)

$(LIB): $(LIB_OBJ)
	$(AR) rcs $@ $^

$(EXAMPLE): examples/version.c $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LIB)

$(TEST_BIN): %$(EXE): %.c $(LIB)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(TEST_SUPPORT) $(LIB)
	./$@

test: $(TEST_BIN)
	@$(MAKE) --no-print-directory clean

clean:
	@$(RM) $(LIB) $(LIB_OBJ) $(EXAMPLE) $(TEST_BIN)
