# Exercise Sheet 9

## Task 1

Create a program that spawns two threads. The first thread pushes 10 million integers with value 1 into a queue (you may reuse the queue implementation from [exercise06 task2](../exercise06/task_2/myqueue.h)), followed by a single 0, then exits. The other thread pops elements from the queue, adds them to a local sum until it encounters a 0, and prints the sum and exits.

Synchronize access to the queue using a mutex. However this time, instead of using a pthread mutex, implement your own mutex using atomics. Your solution should at least provide two functions: `my_mutex_lock` and `my_mutex_unlock`. You may come up with additional functions, if needed.

Think about ways of representing the state of your mutex using an atomic variable. Instead of manipulating that variable directly however, use the `atomic_flag_test_and_set` and `atomic_flag_clear` functions (why is this necessary?). 

_Alternatively_, if you prefer, you can also use `atomic_compare_exchange_weak`. Make sure you fully understand the behavior of these functions and how they could be used to implement your mutex before proceeding.

We want to measure the performance of our own mutex vs. the mutex provided by the system. Provide both variants (using your own mutex and `pthread_mutex_t`) in your uploaded solution. An elegant way of doing this while minimizing code duplication could be to use preprocessor macros, like so:

```c
#if USE_MY_MUTEX
    #define MUTEX_LOCK my_mutex_lock
    // ... other functions
#else
    #define MUTEX_LOCK pthread_mutex_lock
    // ... other functions
#endif
```

You can then use the `MUTEX_LOCK` macro for both, and switch between the two implementations by adding `-DUSE_MY_MUTEX=<0 or 1>` to your compiler call.

Measure the execution time with `/usr/bin/time -v`. Report your findings and interpret the results in `task_1.txt` or `task_1.md`.

**Hints**:

- Locking your mutex will likely require a loop of some sort.
- It may be helpful to `assert` your expectations about the state of the mutex within your lock/unlock functions.
- _Optional_: If you want to improve the performance of your mutex, take a look at the `sched_yield` function.

## Task 2

In this task you'll simulate a simple dice rolling game using **only** *pthread barriers* for synchronization.
Start by reading the man pages on how to use barriers, `pthread_barrier_init(3)` and `pthread_barrier_wait(3)`.

The game is started with `n` players, specified as a command line argument.
Each round, all players roll two dice (simulate this by generating two random numbers between 1 and 6 and summing them up, resulting in a total between 2 and 12).
The player(s) with the highest score are then eliminated from the game.
If all remaining players roll the same number, the round is repeated until a winner has emerged.
The game is finished if only one player is left.

- The main thread **only** does setup and cleanup.
- Each player is simulated by one dedicated thread.
- One of the players (**not the main thread**) determines which player(s) are eliminated.
  Use `pthread_barrier_wait`'s return value for this.
- You **must not** use any global variables!
- It might be helpful to define a struct to combine multiple arguments that are passed to a thread function.
- You might also want to use a struct to save your game state. This could look something like this:

```c
typedef struct {
  uint32_t n;
  volatile int rolls[];
} game_state;
```

Example output where player 3 wins the game:

```
$ ./task_2 5
Player 0 rolled a 1 and a 1, summing up to 2
Player 1 rolled a 2 and a 3, summing up to 5
Player 2 rolled a 6 and a 6, summing up to 12
Player 3 rolled a 3 and a 4, summing up to 7
Player 4 rolled a 6 and a 5, summing up to 11
Eliminating player 2
---------------------
Player 0 rolled a 4 and a 5, summing up to 9
Player 1 rolled a 1 and a 1, summing up to 2
Player 3 rolled a 2 and a 1, summing up to 3
Player 4 rolled a 1 and a 4, summing up to 5
Eliminating player 0
---------------------
Player 1 rolled a 5 and a 3, summing up to 8
Player 3 rolled a 6 and a 2, summing up to 8
Player 4 rolled a 4 and a 4, summing up to 8
Repeating round
---------------------
Player 1 rolled a 4 and a 5, summing up to 9
Player 3 rolled a 6 and a 5, summing up to 11
Player 4 rolled a 6 and a 5, summing up to 11
Eliminating player 3
Eliminating player 4
---------------------
Player 1 has won the game
```

**Note:** The pthread barrier functionality will only compile if you define the right feature test macro. In this case, you can define `-D_DEFAULT_SOURCE` when compiling your code.

## Task 3

In this task, you'll **implement your own version of `pthread_barrier_t`** using only `pthread_mutex_t` and `pthread_cond_t`. 
The goal is to understand barrier synchronization by recreating the basic functionality of a barrier from scratch.

> ✱ Before starting, make sure you're familiar with how barriers work by reviewing the POSIX `pthread_barrier(3)`.

Your implementation must support:

* A fixed number of participating threads, specified at initialization `my_pthread_barrier_init(...)`.
* Waiting until all threads have reached the barrier before continuing `my_pthread_barrier_wait(...)`.
* A special return value (e.g., `PTHREAD_BARRIER_SERIAL_THREAD`) for one thread, to be used for coordination (e.g., leader election).
* Safe reuse of the barrier across multiple rounds.
* Protection against spurious wake ups.
* Safely destroy the barrier before the program exits using `my_pthread_barrier_destroy(...)`.
* You **don't** need to support attributes for the `init` function in any form and can safely assume `NULL` as the only option here.

You must **not** use any part of the pthread barrier API in your implementation — only use `mutexes` and `condition variables`.

Start by defining your barrier struct (defined in [my_pthread_barrier.h](./task_3/my_pthread_barrier.h)):

```c
typedef struct {
  // TODO: implement
} my_pthread_barrier_t;
```

And implement the following three functions (implement in [my_pthread_barrier.c](./task_3/my_pthread_barrier.c)):

```c
int my_pthread_barrier_init(my_pthread_barrier_t *barrier, UNUSED_PARAM void *attr, int count);
int my_pthread_barrier_wait(my_pthread_barrier_t *barrier);
int my_pthread_barrier_destroy(my_pthread_barrier_t *barrier);
```

> Hint: Use a generation counter to ensure that threads from different rounds do not interfere with each other. Make sure your implementation is reusable and robust against race conditions.

Now **re-implement Task 2** using your own barrier library (`my_pthread_barrier_t`).

---

Submit your solution as a zip archive via OLAT, structured as follows, where csXXXXXX is your UIBK login name. Your zip archive must not contain binaries.

```text
exc09_csXXXXXX.zip
├── Makefile             # optional
├── group.txt            # optional
├── task_1
│   ├── Makefile
│   ├── myqueue.h
│   ├── task_1.c
│   └── task_1.txt       # or .md 
├── task_2
│   ├── Makefile
│   └── task_2.c
└── task_3
    ├── Makefile
    ├── my_pthread_barrier.c
    ├── my_pthread_barrier.h
    └── task_3.c
```

Requirements

- [ ] Any implementation MUST NOT produce any additional output
- [ ] If you work in a group, create a `group.txt` file according to the format specified below
- [ ] Auto-format all source files
- [ ] Check your submission on ZID-GPL
- [ ] Check your file structure (and permissions!)
- [ ] Submit zip
- [ ] Mark solved exercises in OLAT

If you worked in a group, the `group.txt` file must be present
and have one line per student which contains the matriculation number
in the beginning, followed by a space and the student's name.
For example, if the group consists of Jane Doe,
who has matriculation number 12345678,
and Max Mustermann, who has matriculation number 87654321,
the `group.txt` file should look like this:

```text
12345678 Jane Doe
87654321 Max Mustermann
```
