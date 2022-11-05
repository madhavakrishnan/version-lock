CUR_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))


CFLAGS += -std=c++14 
CFLAGS += -march=native -mtune=native -O0
CFLAGS += -g -ggdb3
CFLAGS += -Werror -Wall
CFLAGS += -Wno-attributes -Wno-unused-parameter -Wno-unused-value
CFLAGS += -Wno-unused-function -Wno-packed-not-aligned
LDFLAGS += $(MEMMGR) -lpthread 

DEPS_DIR  := $(CUR_DIR)/.deps
DEPCFLAGS = -MD -MF $(DEPS_DIR)/$*.d -MP

SRC_FILES = $(wildcard *.cc)
OBJS_DIR  = $(CUR_DIR)/.objs
OBJ_FILES = $(addprefix $(OBJS_DIR)/, $(SRC_FILES:.cc=.o))
BINS = test

all: $(BINS)
	@for d in $(BINS); \
                do ( cp $$d $(BIN_DIR); \
                ); \
        done

test: $(OBJ_FILES) 
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJS_DIR)/%.o: %.cc $(DEPS_DIR) $(OBJS_DIR)
	$(Q)$(CXX) $(CFLAGS) $(DEPCFLAGS) -c -o $@ $<

$(OBJS_DIR):
	$(Q)mkdir -p $(OBJS_DIR)

$(DEPS_DIR):
	$(Q)mkdir -p $(DEPS_DIR)

clean:
	$(Q)rm -rf $(CUR_DIR)/.deps*
	$(Q)rm -rf $(CUR_DIR)/.objs*

.PHONY: all clean
