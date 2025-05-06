# Exercise Sheet 7

## Task 1

Programs frequently use _system calls_ to interact with the operating system, for example when spawning a child process or when performing everyday file operations.
It sometimes can be useful to inspect the system calls performed by a program, e.g. to debug its behavior when no source code is available, or simply to find out more about how it works.
Linux systems provide the `strace` utility to do just that.

Alongside this document you will find a binary file called [`secret`](task_1/secret), which has been compiled to run on ZID-GPL. You need to run the binary on ZID-GPL.

Begin by reading `man strace` to familiarize yourself with its output format.
Then, use `strace` to investigate the behavior of the provided binary. What is the binary trying to do? 

Try to use the binary as intended and report your findings in `task_1.txt` or `task_1.md`.

## Task 2

In the provided file [task_2.c](task_2/task_2.c) you can find an
implementation of the classic
[Dining philosophers problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem).
However, the implementation is flawed: In some situations multiple philosophers
will wait forever for a chopstick being released, constituting a
**deadlock**.

Answer the following questions in `task_2.txt` or `task_2.md`:

- Explain the program.
- How can a deadlock occur? Does it always happen?
- Change the program to prevent any potential deadlocks from occurring.
  Submit your solution.
  Explain how your changes prevent deadlocks.
- How can you test your solution to ensure that it is deadlock-free?
  - Is it possible with this test to miss deadlocks?
- What are the advantages and disadvantages of your solution?
- Does your solution need additional synchronization?

## Task 3

In this exercise you will implement a [`thread pool`](https://en.wikipedia.org/wiki/Thread_pool), containing a limited number of worker threads.

The provided code in [`task_3.c`](task_3/task_3.c) computes a fixed amount of jobs.

> This is not important, but for your interest: The job is computing the average time you get all the same symbols when playing on a slot machine:
>
> | 💎 | 🍋 | 7️⃣  | 🎰 (# same symbol on all reels: 173)
>
> If you want to visualize the ouputs, uncomment the following line in `task_3.c`:
> ````c
> //#define PRINT_SPINS 1
> ````
> 
> If you do this, you might want to reduce `NUM_TRIES` as well, otherwise the program will run for a long time.

The file contains a version which spawns several threads.
Compile the code with `make task_3` and run it.
Find out how many threads it spawns.

This file also contains a version which uses a thread pool interface, which is defined in [`thread_pool.h`](task_3/thread_pool.h).
Compiling this (`make task_3a`) will not work because the thread pool functions are not implemented yet.
It is now your task to implement the thread pool interface in [`thread_pool.c`](task_3/thread_pool.c).

### Thread Pool Idea

A _thread pool_ should have the following characteristics:

- The thread pool has a limited number of worker threads that compute the given jobs.
- As long as the thread pool is active so are the threads.
- The pool contains a queue (first-in-first-out), for receiving jobs for the threads.
- The worker threads wait for jobs to become available in the queue.
- When there is a job available, one of the threads takes the job and computes it.
- Submitting a job returns a handle for this job, which allows waiting for its completion.

### Thread Pool Implementation

- To guide your efforts we provide you with a header file [`thread_pool.h`](task_3/thread_pool.h).
- The file defines...
  - ... prototypes for functions that are REQUIRED to be implemented.
  - ... exemplary structs, that need to be completed.
- The file is NOT complete.
- The implementation of the prototypes should be done in a separate file called: [`thread_pool.c`](task_3/thread_pool.c).
- The implementation should be abstracted from the actual job implementation, since the thread pool should not care about which jobs it runs (i.e. `thread_pool.c` should neither include `slot_machine.c|h` nor `task_3.c`).
- The thread pool should be implemented without changing the `slot_machine.c|h` and `task_3.c` files (except if you want to change the constants `NUM_JOBS`, `NUM_TRIES`, or `PRINT_SPINS`).
 
**Prototypes**:

- The `void pool_create(thread_pool* pool, size_t size)` function initializes a `thread_pool` by starting `size` worker threads and initializing a queue for jobs.
  Each worker thread continuously checks the queue for submitted jobs.
  Whenever a job is available, (exactly) one worker thread removes it from the queue and runs it.
- The `job_id pool_submit(thread_pool* pool, job_function start_routine, job_arg arg)` submits a job to the thread pool and returns a `job_id`.
- The `void pool_await(job_id id)` function waits for the job with the given `job_id` to finish.
- The `void pool_destroy(thread_pool* pool)` shuts down the thread pool and frees all associated resources.
  Worker threads finish the currently running job (if any) and then stop gracefully.

**Job Specification**:

The `job_function` and `job_arg` types are defined as follows, similar to the arguments for `pthread_create`:

```c
typedef void* (*job_function)(void*);
typedef void* job_arg;
```

In [`task_3.c`](task_3/task_3.c), we use `void* simulate_all_symbols_equal(void* arg)` as the job function.
Job functions take a single argument.
You can ignore the return value of the job function.
Results can be returned by writing to memory specified by a pointer provided as the argument, like it is done in [`task_3.c`](task_3/task_3.c).

**Note**: The provided values for `NUM_JOBS` and `NUM_TRIES` should be used for benchmarking, but you might want to use smaller values during development.


Complete the following tasks:

- Choose suitable data types for the thread pool implementation and implement the functions in [`thread_pool.c`](task_3/thread_pool.c).
- Experiment with different thread pool sizes (change the `POOL_SIZE` constant).
- Compare the performance of `task_3` and `task_3a` (with the optimal thread pool size) by benchmarking them with `/usr/bin/time -v`.
- Answer these questions and submit them in a file called `task_3.txt` or `task_3.md`:
  - How many threads do `task_3` and `task_3a` use, respectively? 
  - After you experimented with different thread pool sizes, which size results in the best performance? Why?
  - Report and interpret the results of your benchmarks. Which discoveries did you make?
  - What are the advantages of using a thread pool rather than spawning a new thread for each job?
  - In which situations does it make sense to use a thread pool?

---

Submit your solution as a zip archive via OLAT, structured as follows, where csXXXXXX is your UIBK login name. Your zip archive **must not** contain binaries.

```text
exc07_csXXXXXX.zip
├── Makefile             # optional
├── group.txt            # optional
├── task_1
│   └── task_1.txt       # or .md
├── task_2
│   ├── Makefile
│   ├── task_2.c
│   └── task_2.txt       # or .md
└── task_3
    ├── Makefile
    ├── slot_machine.c
    ├── slot_machine.h
    ├── task_3.c
    ├── task_3.txt       # or .md
    ├── thread_pool.c
    └── thread_pool.h
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
