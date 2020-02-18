# Target library
CC := gcc
lib := libuthread.a

all: $(lib)

## TODO: Phase 1 and Phase 2

$(lib): queue.o sem.o thread.o LinkedListNode.o HashMap.o tps.o
	$(Q) ar rcs $(@) $^

%.o: %.c %.h
	$(Q) $(CC) $(FLAGS) -c $<

clean:
	$(Q) rm -f *.gch *.d $(lib) *.o