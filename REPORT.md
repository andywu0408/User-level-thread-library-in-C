## User-level thread library (2)

The goal of this project is to extend threads library implemented last time
to support more features, by additionally implementing two independent parts:

* semaphore: a synchronized system for using as a lock or a variable
* thread protected space: a private storage space in each thread if requested

### Implementation

#### semaphore

Semaphores are a way to control the access to common resources by multiple
threads. To keep track of the number of available resource, a semaphore
maintains an internal count, which can be initialized to a certain
positive value when the semaphore is created.

##### supported operations

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

##### atomicity

Please note all operations described above with semaphore is atomic since
shared resources might cause race issues and semaphore is used to get rid of
the race issue. In this way, the thread will enter critical section when it
calls any of above functions and exit critical section after they have done
anything sensitive, including blocking and unblocking threads for the waiting
queue and adding/taking 1 to/from the resources.

#### thread protected space

Thread protected space