CC      ?= cc
AR      ?= ar
CFLAGS  ?= -std=c99 -Wall -Wextra -Wpedantic -O2
CPPFLAGS += -Iinclude

EXE :=
ifeq ($(OS),Windows_NT)
EXE := .exe
endif

# Recursively find files with Make functions, rather than a platform shell command.
rwildcard = $(wildcard $(1)$(2)) $(foreach dir,$(wildcard $(1)*/),$(call rwildcard,$(dir),$(2)))

LIB      = libdvbs2.a
SRC      := $(sort $(call rwildcard,src/,*.c))
LIB_OBJ  := $(SRC:.c=.o)
EXAMPLE  = examples/dvbs2_version$(EXE)
TEST_SRC := $(sort $(call rwildcard,tests/,test.c))
TEST_BIN := $(patsubst %.c,%$(EXE),$(TEST_SRC))

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
