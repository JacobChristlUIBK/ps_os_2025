# Exercise Sheet 5

## Task 1

In this task we will look at **POSIX** message queues — see `mq_overview(7)`.

The goal of this task is to simulate a task scheduler, with multiple tasks being processed according to their priority. 
A task is calculating the statistics of a given set of `n` integers.
The simulation consists of two executables: The `scheduler` receives tasks via a **message queue** and "prints" progress and statistics of the numbers by writing them to `stdout`.
One or more clients, simply called `service`, submit tasks to the message queue.

Scheduler behavior:

- The scheduler receives the name of the message queue as its only argument.
  - To avoid naming collisions with other students on ZID-GPL, we recommend naming your queue based on your user name. (e.g. /csXXXX)
- It creates the message queue and continuously waits for incoming tasks, taking priorities into account.
- Upon receiving a task, it prints `Scheduling task with priority <priority>:`, followed by 
  - A progress in percent (while running) 
    - (Hint you can use something like this: `printf("\rStatistics progress: %d%%", (100 * (i + 1) / n));`)
    - This code uses a `\r`, which is a carriage return. It moves the cursor to its position, so the next printf overwrites the previous line.
  - The `min`, `max`, and `mean` of the `n` numbers (`strtok(3)` and `atoi(3)` might be useful).
- Simulate computation times by sleeping 500 milliseconds in each iteration. As soon as one iteration is done it prints the progress in percent.
    Make sure to flush `stdout` to see the progress steadily being printed.
- Find a way of **gracefully** shutting down the server.
  You may find _"signaling"_ inspiration in earlier exercise sheets.
  Upon shutdown, the server discards all outstanding jobs, prints `Shutting down.`, cleans up and exits.

Service behavior: 

- The client receives the message queue name and a _priority_ as its arguments.
- It then opens the message queue created by the server, sends a request with its contents read from `stdin` and then immediately exits.
- The request should contain `n` numbers (integers).

Example output:

```text
┌──────────────────────────────────┬─────────────────────────────────────────────┬───────────────────────────────────────────┐
│TERMINAL 1                        │TERMINAL 2                                   │TERMINAL 3                                 │
├──────────────────────────────────┼─────────────────────────────────────────────┼───────────────────────────────────────────┤
│$ ./scheduler "/csXXXX"           │                                             │                                           │
│                                  │$ echo "1 10 5 4 1" | ./service "/csXXXX" 1  │                                           │
│Scheduling task with priority 1:  │                                             │                                           │
│Statistics progress: 100%         │                                             │                                           │
│Max: 10, Min: 1, Mean: 4.20       │                                             │$ echo "5 4 3 2 1" | ./service "/csXXXX" 10│
│                                  │$ echo "10 5 4 7 1" | ./service "/csXXXX" 99 │                                           │
│Scheduling task with priority 99: │                                             │                                           │
│Statistics progress: 100%         │                                             │                                           │
│Max: 10, Min: 1, Mean: 5.40       │                                             │                                           │
│                                  │                                             │                                           │
│Scheduling task with priority 10: │                                             │                                           │
│Statistics progress: 100%         │                                             │                                           │
│Max: 5, Min: 1, Mean: 3.00        │                                             │                                           │
└──────────────────────────────────┴─────────────────────────────────────────────┴───────────────────────────────────────────┘
```

In this example, the scheduler starts calculating the statistics of the numbers received by the task with priority 1, as it is the first to be received.
While this computation is done, 2 other requests are added to the queue.
After the first task has been completed, the scheduler takes the next message (according to priority) from the queue.

Additional notes:

`Attention: POSIX message queues are from the POSIX Realtime Extension, which isn't part of the POSIX core. 
Thus, macOS doesn't implement it. 
Students using macOS need to do this exercise in a VM or on zid-gpl.`

- Pick a reasonable message size (static)
- Ensure the message queue is properly cleaned up on shutdown
- Make sure to use appropriate file access modes

Answer the following questions, in a file task_1.txt or task_1.md, provide this file additionally to the source codes.

- What is the range of possible priorities defined by POSIX?
  - Are you limited to this range on your system?
  - What is more important a task with priority 2 or 99 (gets scheduled first)?

## Task 2

In this task, you will develop a multi-process application using shared memory for communication.

Begin by reading `shm_overview(7)`. Take note of additional linker flags that may be required.

The program receives three integers as arguments of type `unsigned long long`:

- `N`, an arbitrary integer.
- `K`, number of reads/writes to the buffer.
- `L`, the length of the circular buffer (total size: `L * sizeof(unsigned long long)`).

First, each input argument is parsed.
Then, it sets up shared memory for communication.
It contains a [circular buffer](https://en.wikipedia.org/wiki/Circular_buffer) and one element for the result.
We provided you with a sample struct (see below).
Feel free to use it.

```c
struct shared_data {
    unsigned long long result;   // final result
    unsigned long long buffer[]; // circular buffer
};
```

_Hint_: In this struct, `buffer` is a so-called [flexible array member](https://en.wikipedia.org/wiki/Flexible_array_member).

The circular buffer allows the number of reads/writes defined by `K` to be larger than the number of elements `L` in the buffer.
If the end is reached, the process restarts at the front (using `buffer[i % L]`, where `i` is the number of iterations done so far).

Next, two child processes are created (using `fork`), which act as a **Producer** and a **Consumer**, respectively.
The processes run in parallel and perform calculations on the buffer:

- **Producer**:
  - The process loops `K` times, starting from 0.
  - In each iteration `i`, the number `N * (i + 1) + 17` is written into the position `i % L` of the circular buffer.
  - It then finishes up, and returns success.
- **Consumer**:
  - The process computes the sum of each element in the circular buffer.
  - It prints the final result, and writes it into the `result` member in the shared memory.
  - It then finishes up, and returns success.

The parent process waits for the termination of both child processes.
It then reads the result of their computation from the `result` member in the shared memory.
Afterwards, it validates the result of the computation using the provided `validate_result` function (see below).
Finally, it finishes up, and returns success.

_Hint_: Make sure to properly clean up the shared memory.

```c
void validate_result(unsigned long long result, const unsigned long long K, const unsigned long long N) {
    for (unsigned long long i = 0; i < K; i++) {
        result -= N * (i + 1) + 17;
    }
    printf("Checksum: %llu\n", result);
}
```

### Example outputs

```sh
$ ./task_2 32044 3042 3040
Result: 148312597446
Checksum: 0 
```

```sh
$ ./task_2 32044 3042 3040
Result: 148507424966
Checksum: 194827520 
```

You will notice that the result is not always correct.
This is by design and does not necessarily indicate an implementation mistake.

### Discussion

Test your implementation with different values for `N`, `K`, and `L`.
Then, answer the following question inside a `task_2.txt` or `task_2.md` file:

- What is the checksum indicating?
- Which values are correct vs. incorrect?
- How can the checksum be used to tell if a result is correct or not?

### Additional notes and hints

**Macro support**:

In order to use macros such as `MAP_ANONYMOUS`, you have to define the `_DEFAULT_SOURCE` feature test macro.
This can be done by adding the compile flag `-D_DEFAULT_SOURCE`.
See the `feature_test_macros(7)` man page for more information.

**Quality Assurance**:

- Make sure to perform proper error handling and input parsing.
- Test your implementation using `valgrind`.
  - Did you clean everything up?
  - Do you have access to everything you need?
  - Is everything correctly initialized?

## Task 3

In this task, you will use _semaphores_ to synchronize the access to the shared data structure.

This exercise MUST be implemented using the POSIX semaphore facilities — see `sem_overview(7)`.

`Attention: POSIX semaphores are from the POSIX Realtime Extension, which isn't part of the POSIX core. 
Thus, macOS doesn't implement it. 
Students using macOS need to do this exercise in a VM or on zid-gpl.`

### Task 3a

Implement the same program as in the previous task 2. This time, however, ensure that the checksum is always correct by utilizing synchronization concepts.

Use *two* semaphores to implement this behavior.

Your program should now always compute the correct checksum.

**Important**: Make sure that the processes can perform work on the circular buffer _concurrently_, i.e., the semaphores must be used for _counting_.

### Task 3b

Answer the following questions in `task_3b.txt` or `task_3b.md`:

1. What do you notice when measuring the execution time of your solutions for Task 2 and Task 3a using `/usr/bin/time`?
2. What is the difference between shared memory and a global variable after calling `fork()`?
3. What is a race condition? Why do race conditions occur?
4. Is the output of a program with a race condition always incorrect?
5. If you run your program 100 times and always get the correct result, does that mean that there is no race condition?
6. What is a critical section? What role does it play in preventing race conditions?
7. What is synchronization?
8. Why are semaphores well suited for synchronizing such programs?
9. Could you also just use *one* semaphore to ensure correct execution of this program? Is there a disadvantage when only using one semaphore?
10. What would you need to do in order to support a *second* consumer process such that each consumer computes its own (local) sum and then the parent process computes the final sum after all the children have exited? Is your approach efficient?
11. There are two ways of creating a POSIX semaphore. Which would you use when?

---

Submit your solution as a zip archive via OLAT, structured as follows, where csXXXXXX is your UIBK login name. Your zip archive **must not** contain binaries.

```text
exc05_csXXXXXX.zip
├── Makefile             # optional
├── group.txt            # optional
├── task_1
│   ├── Makefile
│   ├── task_1.txt       # or .md
│   ├── scheduler.c
│   └── service.c
├── task_2
│   ├── Makefile
│   ├── task_2.c         
│   └── task_2.txt       # or .md
└── task_3
    ├── Makefile
    ├── task_3a.c
    └── task_3b.txt      # or .md
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
