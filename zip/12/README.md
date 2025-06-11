# Exercise Sheet 12

The following is a _real_ exam from a previous year.
It should give you a general idea of what to expect from this year's exam.

This exercise sheet is optional, and provides up to 3 **bonus points**:
These points are not included in the total number of points reachable this semester (33), but will count towards your personal final score (theoretically, it is thus possible to achieve 36/33 points).

While the exam itself is not split into explicit tasks, we define the following tasks for receiving the bonus points:

- **Task 1:** Implement the flower field.
- **Task 2:** Implement the bees.
- **Task 3:** Implement the functionality for a possible bear attack.

**As usual, please mark your completed tasks in OLAT**.

**NOTE**: We recommend that you attempt to solve this exercise sheet by simulating the real exam scenario:
Prepare any materials you may need, such as your solutions to previous exercises, for quick access (the real exam will be open book!).
Then try to solve this mock exam within an hour and 30 minutes.

---

# Exam

## General Information

**Please read this description carefully.**

Talking, chatting, texting, or any other form of live human-to-human or human-to-AI communication is not allowed and may lead to a failing grade.
However, you are allowed to use all other offline and online resources such as previous homework solutions, man pages, StackOverflow, etc.

If you directly copy code from one of these sources, add a **citation** as a comment before the code fragment. E.g.

```c
// The following code is from my solution to exercise 5, task 2.
```

or

```c
// Source: https://stackoverflow.com/questions/14888027/mutex-lock-threads
```

If we find the same code in multiple solutions without citation, we have to assume cheating, and you will receive a failing grade.
It does not matter whether you are the person copying from or providing your solution to another, both will receive a failing grade.

**Requirements**:

  - You **must not** use global variables (except a single `volatile sig_atomic_t` variable for your signal handler).
  - All errors **must** be handled. Output them to `stderr` and exit with `EXIT_FAILURE`.
  - Additional debug output **must** be removed/disabled before submission.
  - Your program **must** include a `Makefile` and should compile successfully on ZID-GPL.
  - If you use threads/processes, they **must** be created using `pthread_create(3)`/`fork(2)`. You are **not** allowed to create unnecessary threads/processes to solve the given tasks. 
  - Make sure that your program is able to achieve a sensible amount of concurrency and does **not** waste an unreasonable amount of resources.
  - Make sure to correctly clean up all used resources. Use `valgrind` to check for memory leaks.
  - Make sure your program does not contain any race conditions or deadlocks; `valgrind --tool=helgrind` may help.

Points will be deducted if you do not meet these requirements.

If something does not work as expected, provide comments in the source code explaining what you think the problem is.
It might improve your grade.

**You have 1 hour and 30 minutes to complete this exercise.**

**Please make sure to upload your solution to OLAT in time.**

### Hints:

- Develop your program incrementally, and focus on the core functionality first.
- You may want to define the `_DEFAULT_SOURCE` feature test macro — see `feature_test_macros(7)`.
- Passing the flags `--leak-check=full --show-leak-kinds=all --track-origins=yes` to `valgrind` might help you finding further memory related issues.
- Remember to pass the `-g` flag to your compiler in order to get proper information when using debugging tools.
- Assertions may be helpful during development — see `assert(3)`.

# Task: Beehive

Create a program `beehive` that simulates the nectar collection of bees in a flower field of a specific size. The program receives three arguments: `./beehive <flower field width> <flower field height> <number of bees>`.

**Flower field**

- A flower field is a grid where each element corresponds to exactly one flower (`<flower field width> * <flower field height> = total amount of flowers`).
- Each flower in the field can be harvested exactly once. Once a bee collects nectar from a flower, that flower no longer contains any nectar. Initially, all flowers in the field contain nectar.

**Bees**

- Each bee is represented as a separate pthread. Make sure to properly synchronize the program.
- Bees share information about possible food sources through a queue. You can use the queue implementation provided in `exercise06`, but you might have to change its value type.
- A bee can either (i) fly to a food source, collect nectar, and return to the hive, or (ii) if there is no food source available, it stays in the hive and works there.
    - (i) If there is a food source in the queue, the bee flies to that location, taking a random amount of time in the interval `[100,500]` milliseconds. If a harvestable flower is found at that location, the bee collects nectar and checks its surrounding cells (left, right, top, bottom - you can assume that flowers beyond the border are already harvested) for more harvestable flowers. All discovered harvestable flowers are added to the queue. Several bees could report the same flower as harvestable, i.e. you do not need to remove duplicates in the queue. If the flower at the location has already been harvested, the bee returns to the hive without informing other bees about food sources.
    - (ii) If there are no food sources in the queue, the bee works in the hive a random amount of time in the interval `[100,500]` milliseconds.
- To guide the first bee to a flower, you can randomly place a food source in the queue.
- After completing its tasks, a bee checks for further work: again either collect nectar or work in the beehive.
- The program terminates once nectar from all flowers has been collected.

Once you have implemented this basic functionality, extend your simulation with a possible *bear attack*:

- After each bee completes one task (whether it's nectar collection or working in the beehive), bees wait for each other and collectively check if there is a bear outside.  With a chance of `10%` the bees encounter a bear and fight it. With a chance of `50%` either the bees repel the bear and continue with their work, or the bear destroys the beehive and the program terminates.
- At the end of the simulation, print a summary and report if the beehive was destroyed by the bear.

**Example output**

```text
$ ./beehive 10 10 3
Bee 0 is flying to food source at position (8,0).
Bee 1 is working in beehive.
Bee 2 is working in beehive.
Bee 0 collected nectar at position (8,0) and reports potential food sources: (7,0)(9,0)(8,1).
Bees encounter a bear and engage in a fight.
The bees successfully repel the bear and resume their work.
Bee 1 is flying to food source at position (7,0).
Bee 0 is flying to food source at position (9,0).
Bee 2 is flying to food source at position (8,1).
Bee 0 collected nectar at position (9,0) and reports potential food sources: (9,1).
Bee 2 collected nectar at position (8,1) and reports potential food sources: (7,1)(9,1)(8,2).
Bee 1 collected nectar at position (7,0) and reports potential food sources: (6,0)(7,1).
Bee 2 is flying to food source at position (9,1).
Bee 0 is flying to food source at position (7,1).
Bee 1 is flying to food source at position (9,1).
Bee 1 collected nectar at position (9,1) and reports potential food sources: (9,2).
Bee 0 collected nectar at position (7,1) and reports potential food sources: (6,1)(7,2).
Bee 2 could not find nectar at position (9,1).
Bees encounter a bear and engage in a fight.
Bear destroys the beehive.
3 bees collected nectar from 6/100 flowers.
Beehive was destroyed.
```

---

Submit your solution as a zip archive via OLAT, structured as follows, where csXXXXXX is your UIBK login name. Your zip archive must not contain binaries.

```text
exc12_csXXXXXX.zip
├── Makefile             # optional
├── group.txt            # optional
├── task_1
│   ├── Makefile
│   ├── beehive.c   
│   └── myqueue.h
├── task_2
│   ├── Makefile
│   ├── beehive.c   
│   └── myqueue.h
└── task_3
│   ├── Makefile
│   ├── beehive.c   
└── └── myqueue.h
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

