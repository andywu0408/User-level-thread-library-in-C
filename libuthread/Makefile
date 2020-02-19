# Target library
CC := gcc
lib := libuthread.a
FLAGS  := -Wall -Wextra -Werror

generated_object_files := sem.o LinkedListNode.o HashMap.o tps.o
provided_object_files := thread.o queue.o

ifneq ($(V),1)
Q = @
endif

ifneq ($(D),1)
FLAGS += -O2
endif

all: $(lib)

deps := $(patsubst %.o, %.d, $(lib))
DEPFLAGS = -MMD -MF $(@:.o=.d)

$(lib): $(generated_object_files) $(provided_object_files)
	$(Q) ar rcs $(@) $^

%.o: %.c %.h
	$(Q) $(CC) $(FLAGS) -c $<

clean:
	$(Q) rm -f $(generated_object_files) $(lib)