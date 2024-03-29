# Target programs
#  **Add more lines to this variable in order to compile more programs**
programs := \
	sem_count.x \
	sem_buffer.x \
	sem_prime.x \
	tps_simple.x \
	tps_read_char_input.x \
	tps_complex_tester_1.x \
	tps_destroy_before_creation.x \
	tps_double_create.x \
	tps_invalid_offset.x \
	tps_read_write_before_creation.x \
	tps_read_write_null_buffer.x \
	tps_read_write_various_offsets.x \
	tps_read_int_input.x \
	tps_complex_tester_2.x \
	tps_read_unsigned_char_input.x \
	tps_read_double_input.x \
	tps_testsuite.x \
	HashMap_tester.x

## *** IMPORTANT *** ##
##	You should NOT have to modify anything below
##
##  (but you are encouraged to read it and understand)
## ***************** ##

# User-level thread library
UTHREADLIB := libuthread
UTHREADPATH := ../$(UTHREADLIB)
libuthread := $(UTHREADPATH)/$(UTHREADLIB).a


# Default rule
all: $(libuthread) $(programs)

# Avoid builtin rules and variables
MAKEFLAGS += -rR

# Don't print the commands unless explicitly requested with `make V=1`
ifneq ($(V),1)
Q = @
V = 0
endif

# Current directory
CUR_PWD := $(shell pwd)

# Define compilation toolchain
CC	= gcc

# General gcc options
CFLAGS	:= -Wall -Werror
CFLAGS	+= -pipe
CFLAGS	+= -pthread
## Debug flag
ifneq ($(D),1)
CFLAGS	+= -O2
else
CFLAGS	+= -g
endif

# Linker options
LDFLAGS := -L$(UTHREADPATH) -luthread
tps_testsuite.x: LDFLAGS += -Wl,--wrap=mmap

# Include path
INCLUDE := -I$(UTHREADPATH)

# Generate dependencies
DEPFLAGS = -MMD -MF $(@:.o=.d)

# Application objects to compile
objs := $(patsubst %.x,%.o,$(programs))

# Include dependencies
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

# Rule for libuthread.a
$(libuthread):
	@echo "MAKE	$@"
	$(Q)$(MAKE) V=$(V) D=$(D) -C $(UTHREADPATH)

# Generic rule for linking final applications
%.x: %.o $(libuthread)
	@echo "LD	$@"
	$(Q)$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Generic rule for compiling objects
%.o: %.c
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $< $(DEPFLAGS)

# Cleaning rule
clean:
	@echo "CLEAN	$(CUR_PWD)"
	$(Q)$(MAKE) V=$(V) D=$(D) -C $(UTHREADPATH) clean
	$(Q)rm -rf $(objs) $(deps) $(programs)

.PHONY: clean $(libuthread)

