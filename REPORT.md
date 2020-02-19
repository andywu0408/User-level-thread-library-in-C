## User-level thread library (2)

The goal of this project is to extend threads library implemented last time
to support more features, by additionally implementing two independent parts:

* semaphore: a synchronized system for using as a lock or a variable
* thread protected storage: a private storage space in each thread if requested

### Implementation

#### Semaphore

Semaphores are a way to control the access to common resources by multiple
threads. To keep track of the number of available resource, a semaphore
maintains an internal count, which can be initialized to a certain
positive value when the semaphore is created.

##### Supported operations

A waiting queue is maintained in the library to control the access to the
common resource.

* create: `sem_t sem_create(size_t count)` creates a semaphore with a count
taken in as an initialized common resource, but must be a non-negative number
and returns the pointer to newly created semaphore.
* destroy: `int sem_destroy(sem_t sem)` destroys the given semaphore and free
the allocated memory, and returns 0 if destroyed successfully; -1 otherwise.
* up: `int sem_up(sem_t sem)` increments the common resource by 1. It will
free the next thread in the waiting queue if there is any, since the common
resource for the threads in the waiting queue are now available. The function
returns -1 if @sem is null, 0 otherwise.
* down: `int sem_down(sem_t sem)` is assumed to decrement the common
resource by 1. However, if there is no resources at all, the thread will be
blocked and put into the waiting queue and start waiting their share of
common resources. The function returns -1 if @sem is null, 0 otherwise.
* get_value: `int sem_getvalue(sem_t sem, int *sval)` gets the call the
number of common resources that is maintained in the semaphore and put into
@sval. The function returns -1 if @sem or @sval is null, 0 otherwise.

##### Atomicity

Please note all operations described above with semaphore is atomic since
shared resources might cause race issues and semaphore is used to get rid of
the race issue. In this way, the thread will enter critical section when it
calls any of above functions and exit critical section after they have done
anything sensitive, including blocking and unblocking threads for the waiting
queue and adding/taking 1 to/from the resources.

##### Starvation

In the operation up, if there is some threads in the waiting queue. Only one
thread (first in the queue) will be unblocked, so it is guaranteed this
thread will receive this unit of resource, instead of unblocking all threads
in the waiting queue, in which case, these threads will start to compete one
unit of resources and all keep starving expect the luck one which gets the
resource.

#### Thread protected space

Normally, threads of a same process all share the same memory address space
. It is a good practice, but sometimes some threads want to have some "secret
place" for their data, and do not allow others to modify them. Thread protected
storage (a.k.a TPS) is created upon this demand.

#### Supported operations

A HashMap is implemented and maintained in order to speed up efficiency and
link thread id and their corresponding TPS in the library. Please refer to
@data structures for more details.

* init: `int tps_init(int segv)` will initialized the map and error handler
if specified. It will return -1 if map has already been initialized or any
memory failure happens, 0 otherwise. It is expected for the user to call this
function before the user proceed to use any operations described below.
* create: `int tps_create(void)` will create a tps area for current thread
. It will return -1 if the current thread has a TPS or creation failure, 0
otherwise. The TPS created has no access from anyone.
* destroy: `int tps_destroy(void)` will destroy TPS linked by current thread
in the map. It will return -1 if the current thread doesn't has a TPS or, 0
otherwise.
* read: `int tps_read(size_t offset, size_t length, void *buffer)` will
take in an offset and a length to read from its own TPS and copy to buffer
. When being read, TPS will have a temporary read access opened. The function
returns 0 if read succeeds; -1 if current thread doesn't have a TPS or
 `offset + length` is out of bound.
* write: `int tps_write(size_t offset, size_t length, void *buffer)` will
copy the buffer to TPS by a offset and length. When being read, TPS will have
a temporary write access opened. The function returns 0 if write succeeds; -1
if current thread doesn't have a TPS or `offset + length` is out of bound.
* clone: `int tps_clone(pthread_t tid)` will perform lazy clone. The space of
TPS of current TPS will not created if the current TPS perform reading all
the time. The current thread keeps reading the information in the TPS of the
target @tid. Until current thread tries to write something to the TPS, its own 
thread will be created and copy everything from the target TPS. It will 
eventually save much heap space for the program. The function returns -1 if
current thread already has a TPS or target thread doesn't have a TPS, 0
otherwise.

Please refer to section test and run for more information how these
operations will be tested.

#### Atomicity

The supported operations are also atomic since data in TPS is sensitive and
might cause race issues. The functions will also enter and exit critical
section.

### Data structures

#### Queue

A queue is used in each semaphore object to serve as a waiting list for any
thread tries to get unavailable resources.

The queue is implemented last time of uthread library, and the underlying
structure is LinkedList. Files queue.h, queue.c, LinkedListNode.h, and
LinkedListNode.c are also provided. Please refer to these documents if more
 information is needed.

#### HashMap

A chaining based HashMap is implemented in order to improve efficiency in TSP
against queue.

The HashMap is dynamically resized when 80% of the space is full to avoid
 collision and improve efficiency.

The HashMap is designed to used in this library, so generics is not supported.
It has fixed pair entry as <Key: pthread_t, Value: tps_t object>

##### Basic operations
* create: `map_t HashMap_create()`
* add: `int HashMap_add(map_t map, pthread_t tid, void* tps)` adds a key
-value pair into the map. Rejected (return -1) if key already in the map.
* remove: `int HashMap_remove(map_t map, pthread_t tid)` removes a key-value
 pair based on the key. Rejected if key not in map
* get: `void* HashMap_get(map_t map, pthread_t tid)` gets the value based on
 the given key. Rejected (returns null) if key not in map.
* size: `int HashMap_size(map_t map)` returns the size of map. Rejected if
 map is null
* getKeys: `pthread_t* HashMap_getKeys(map_t map)` returns an array of keys
 (pthread_t). Rejected (returns null) if map is null.
* getValues: `void** HashMap_getValues(map_t map)` return an array of values
. Rejected (returns null) if map is null.
* destroy: `int HashMap_destroy(map_t map)` frees the memory allocated for the
 map. Rejected (returns -1) if map is null.

WARNING: operations are not thread-safe.

##### Time complexity comparison with Queue
|Operations|Queue|HashMap|
|----------|-----|-------|
|search for a pthread_t|O(n)|O(1)|
|add a pthread_t       |O(1)|O(1)|
|remove a pthread_t    |O(n)|O(1)|
|get all pthread_t     |O(n)|O(n)|

HashMap is evaluated with average time instead of worst case.

#### TPS_t
a customized object encapsulate `void* page` pointer to the TPS and an object
 to clone flag for lazy cloning.
 
### Test and Run

#### Run

A Makefile has been provided along with source code. The Makefile uses all
flags as specified by the prompt. Use `make` to compile the library
`libuthread.a`. Use the library in any code and link the `libuthread.a
` when compiling. After running, use `make clean` to remove intermediate
files and the library.

#### Test

#### Test for HashMap

Instead of several unit tests, a comprehensive test file is created
, including all external operations and inner methods like chaining, resizing
. Please refer to `HashMap_tester.c` for more details

#### Test for semaphore

All provided test cases has passed. We try to modify the given input and they
 all work as expected.

#### Test for TSP

TODO 