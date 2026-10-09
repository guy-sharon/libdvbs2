ifeq ($(origin CC), default)
CC := $(shell (command -v cc >/dev/null 2>&1 && echo cc) || (where cc >/dev/null 2>&1 && echo cc) || echo gcc)
endif
AR      ?= ar
CFLAGS  ?= -std=c99 -Wall -Werror -Wextra -Wpedantic -Ofast

# Detect number of CPUs for parallel builds
JOBS := $(shell \
  nproc 2>/dev/null || \
  sysctl -n hw.ncpu 2>/dev/null || \
  cmd.exe /c "echo %NUMBER_OF_PROCESSORS%" 2>/dev/null || \
  echo 4 \
)
MAKEFLAGS += -j$(JOBS)
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
TEST_TIME := tests/time_test.c
TEST_SRC := $(filter-out $(TEST_TIME),$(TEST_SRC))
TEST_SUPPORT := $(shell find tests -type f -name '*.c' ! -name '*_test.c' -print | sort)
TEST_BIN := $(patsubst %.c,%$(EXE),$(TEST_SRC))
TIME_BIN := tests/time_test$(EXE)
PROFILE_DIR := build/profile
PROFILE_LIB := $(PROFILE_DIR)/libdvbs2.a
PROFILE_OBJ := $(patsubst src/%.c,$(PROFILE_DIR)/%.o,$(SRC))
PROFILE_BIN := $(PROFILE_DIR)/time_test
FLAMEGRAPH_DIR ?= $(HOME)/FlameGraph
STACKCOLLAPSE_PERF := $(FLAMEGRAPH_DIR)/stackcollapse-perf.pl
FLAMEGRAPH_SCRIPT := $(FLAMEGRAPH_DIR)/flamegraph.pl
# Set to "sudo perf" when unprivileged perf access is restricted.
PERF ?= sudo perf
FIREFOX ?= firefox

.PHONY: all test clean time profile FORCE_PROFILE $(TEST_BIN)

all: $(LIB) $(EXAMPLE)

$(LIB): $(LIB_OBJ)
	$(AR) rcs $@ $^

$(EXAMPLE): examples/version.c $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LIB)

$(TEST_BIN): %$(EXE): %.c $(LIB)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(TEST_SUPPORT) $(LIB)
	./$@

test: $(TEST_BIN)
	@$(RM) $(LIB) $(LIB_OBJ) $(EXAMPLE) $(TEST_BIN) $(TIME_BIN)

time: $(TIME_BIN)
	clear
	@./$<
	@$(RM) $(LIB) $(LIB_OBJ) $(EXAMPLE) $(TEST_BIN) $(TIME_BIN)

$(TIME_BIN): $(TEST_TIME) $(LIB)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(TEST_SUPPORT) $(LIB)

profile: $(PROFILE_BIN)
	@command -v "$(firstword $(PERF))" >/dev/null || { echo "Error: $(firstword $(PERF)) is required for profiling." >&2; exit 1; }
	@command -v perl >/dev/null || { echo "Error: Perl is required to generate the flamegraph." >&2; exit 1; }
	@test -f "$(STACKCOLLAPSE_PERF)" || { echo "Error: missing $(STACKCOLLAPSE_PERF);\nrun git clone https://github.com/brendangregg/FlameGraph.git "\$$HOME/FlameGraph"" >&2; exit 1; }
	@test -f "$(FLAMEGRAPH_SCRIPT)" || { echo "Error: missing $(FLAMEGRAPH_SCRIPT);\nrun git clone https://github.com/brendangregg/FlameGraph.git "\$$HOME/FlameGraph"" >&2; exit 1; }
	@command -v "$(FIREFOX)" >/dev/null || { echo "Error: $(FIREFOX) is required to open the flamegraph." >&2; exit 1; }
	@cd $(PROFILE_DIR) && { rm -f perf.data && $(PERF) record -g --call-graph fp -o perf.data -- ./time_test || { echo 'Error: perf recording failed; if access is restricted, retry with make profile PERF="sudo perf".' >&2; exit 1; }; }
	@cd $(PROFILE_DIR) && $(PERF) report --stdio -i perf.data > ../../profile.txt
	@cd $(PROFILE_DIR) && $(PERF) script -i perf.data | perl "$(abspath $(STACKCOLLAPSE_PERF))" | perl "$(abspath $(FLAMEGRAPH_SCRIPT))" > ../../flamegraph.svg
	@$(FIREFOX) --new-window "$(abspath flamegraph.svg)" >/dev/null 2>&1 &

$(PROFILE_LIB): $(PROFILE_OBJ)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $^

$(PROFILE_DIR)/%.o: src/%.c FORCE_PROFILE
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -O0 -g -fno-omit-frame-pointer -c -o $@ $<

$(PROFILE_BIN): tests/time_test.c $(TEST_SUPPORT) $(PROFILE_LIB) FORCE_PROFILE
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -O0 -g -fno-omit-frame-pointer -o $@ tests/time_test.c $(TEST_SUPPORT) $(PROFILE_LIB)

FORCE_PROFILE:

clean:
	@$(RM) $(LIB) $(LIB_OBJ) $(EXAMPLE) $(TEST_BIN) $(TIME_BIN) profile.txt flamegraph.svg
	@rm -rf $(PROFILE_DIR)
