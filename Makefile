CUR_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))


CFLAGS += -std=c++17 
CFLAGS += -march=native -mtune=native -O0
CFLAGS += -g -ggdb3
CFLAGS += -Werror -Wall
CFLAGS += -Wno-attributes -Wno-unused-parameter -Wno-unused-value
CFLAGS += -Wno-unused-function -Wno-packed-not-aligned 
LDFLAGS += $(MEMMGR) -lpthread -ltbb

DEPS_DIR  := $(CUR_DIR)/.deps
DEPCFLAGS = -MD -MF $(DEPS_DIR)/$*.d -MP

# .cc files for testing
TEST_SRC_FILES = $(wildcard *.cc)
TEST_OBJS_DIR  = $(CUR_DIR)/.test_objs
TEST_OBJ_FILES = $(addprefix $(TEST_OBJS_DIR)/, $(TEST_SRC_FILES:.cc=.o))
TEST_BIN = test

# .cpp files for benchmark
BENCH_SRC_FILES = $(wildcard *.cpp)
BENCH_OBJS_DIR  = $(CUR_DIR)/.bench_objs
BENCH_OBJ_FILES = $(addprefix $(BENCH_OBJS_DIR)/, $(BENCH_SRC_FILES:.cpp=.o))
BENCH_BIN = ycsb

all: $(TEST_BIN) $(BENCH_BIN)

test: $(TEST_OBJ_FILES) numa-config.h 
	$(CXX) -o $@ $^ $(LDFLAGS)

ycsb: $(BENCH_OBJ_FILES)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(TEST_OBJS_DIR)/%.o: %.cc $(DEPS_DIR) $(TEST_OBJS_DIR)
	$(Q)$(CXX) $(CFLAGS) $(DEPCFLAGS) -c -o $@ $<

$(BENCH_OBJS_DIR)/%.o: %.cpp $(DEPS_DIR) $(BENCH_OBJS_DIR)
	$(Q)$(CXX) $(CFLAGS) $(DEPCFLAGS) -c -o $@ $<

numa-config.h:
		 python3 cpu-topology.py > $(CUR_DIR)/numa-config.h

$(TEST_OBJS_DIR):
	$(Q)mkdir -p $(TEST_OBJS_DIR)

$(BENCH_OBJS_DIR):
	$(Q)mkdir -p $(BENCH_OBJS_DIR)

$(DEPS_DIR):
	$(Q)mkdir -p $(DEPS_DIR)

clean:
	$(Q)rm -rf $(CUR_DIR)/.deps*
	$(Q)rm -rf $(CUR_DIR)/.test_objs*
	$(Q)rm -rf $(CUR_DIR)/.bench_objs*
	$(Q)rm -rf $(CUR_DIR)/numa-config.h 
	$(Q)rm -rf $(CUR_DIR)/$(TEST_BIN)
	$(Q)rm -rf $(CUR_DIR)/$(BENCH_BIN)

.PHONY: all clean
