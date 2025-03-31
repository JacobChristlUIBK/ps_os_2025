# Exercise Sheet 4

## Task 1

Create a program `arg_counter.c` which prints how many input arguments were given. The program should accept maximum 10 arguments (excluding the program name itself).

Example output:

```bash
$ ./arg_counter
Usage: ./arg_counter <arg1> [arg2 ... arg10]
$ ./arg_counter hello world
Result: 2
```

The goal of this exercise is to perform proper *error handling* and return according *exit codes*.

Below is a list of constraints for each available *exit code*:

- return `EXIT_SUCCESS` if the run is successful.
- return `13` if no arguments are provided (i.e., `./arg_counter`). In addition, print usage information.
- return `7` if more than 10 arguments were provided.
- return `99` if the `OFFSET` (see below) contains invalid data (not a number).

For better readability, define an `enum` for non-standard *exit codes*.

Think of at least **one** other constraints and add appropriate *exit codes*.

Extend your program to add the value of an *environment variable* `OFFSET` to the argument count. For example, when `OFFSET` is set to `5`, the call `./arg_counter a b c` should return `8`, as the value of `OFFSET` is added to the total argument count. The program should continue to accept up to 10 arguments. Have a look at `getenv(3)`. In order to test your program, figure out a way to set/unset environment variables in your shell. Additionally, find a way to set the environment variable *only* for a specific command (in this case `./arg_counter` with appropriate arguments).

You can inspect environment variables on your shell with `printenv` and print them with `echo $MY_ENV_VAR`.

Answer the following questions in `task_1.txt`:

- How do you set/unset environment variables in your shell?
- How can you retrieve the returned *exit code* in your shell after running the program?
- In your shell `;`, `&&`, and `||` can be used to execute multiple commands on a single line. What are the differences between these 3 operators?
- What is the `PATH` environment variable and why is it important?

## Task 2

On the shell you can provide the output of a command as input to another using a _pipe_ `|`.

```
$ cat file_1.txt file_2.txt | cut -c 22-
Hello
 __        __  _  _
\ \      / /__| || |
Start
 \ \ /\ / / _ \ || |
  \ V  V /  __/ || |
Good
End
   \_/\_/ \___|_||_|
Repeat
 _____   ____  _   _ ______
Remove
|  __ \ / __ \| \ | |  ____|
| |  | | |  | |  \| | |__
Keep
| |  | | |  | | . ` |  __|
going
Finish
| |__| | |__| | |\  | |____
|_____/ \____/|_| \_|______|
```

This _forwards_ the standard output of `cat` to the standard input of `cut` (remove sections from lines).
The result you see is the content of [`file_1.txt`](task_2/file_1.txt) and [`file_2.txt`](task_2/file_2.txt), minus the first 22 characters.
Multiple commands can be chained together to build longer _pipelines_.

```
$ cat file_1.txt file_2.txt | grep '^;' | cut -c 22-
 __        __  _  _
 \ \ /\ / / _ \ || |
  \ V  V /  __/ || |
   \_/\_/ \___|_||_|
 _____   ____  _   _ ______
|  __ \ / __ \| \ | |  ____|
| |  | | |  | |  \| | |__
| |  | | |  | | . ` |  __|
| |__| | |__| | |\  | |____
|_____/ \____/|_| \_|______|
```

Your task is to create a C program `cat_cut` that accepts two input files as arguments (e.g. `file_1.txt` and `file_2.txt`) and uses `fork` together with `exec` to run the two commands `cat file_1.txt file_2.txt` and `cut` (with the argument `-c 22-`) like shown above.
Have a look at the `fork(2)`, `exec(3)`, `pipe(7)`, `pipe(2)`, and `dup(2)`/`dup2(2)` man pages.

Then, create another program `cat_grep_cut` that mimics the second example.
Your Makefile should build both executables.

If your programs are called as follows they should produce exactly the same output as the shell examples above:

```sh
./cat_cut file_1.txt file_2.txt
./cat_grep_cut file_1.txt file_2.txt
```

There are even more helpful operators built into your shell.
Explain what the following operators do: `<`, `>`, `2>`, `2>&1`. Provide your answers in `task_2.txt` or `task_2.md`.

## Task 3

In this task, you will create an interactive chat app using a server and multiple clients.
To accomplish this, you will work with FIFOs, which are also called **named** pipes.

The goal is to write a simple **server** with a message counter to which multiple interactive **clients** can send string messages.
Upon receiving a message, the **server** will increment its message counter variable by 1, and then print the message counter, client name, and client message.  
Your solution should produce two separate executables, `client` and `server`. 

### Server

1. The **server** can be started with a list of clients that may connect to it, e.g. after starting with `./server jacobi riemann`, two clients named `jacobi` and `riemann` can connect.
2. The server then creates a FIFO for each client. Think about a way of associating each FIFO with a specific client.
   **NOTE**: If you have problems creating FIFOs, make sure that your filesystem actually supports them (this can be a problem in WSL when working on an NTFS drive). A good location to store FIFOs is somewhere inside `/tmp` (when working on ZID-GPL, be sure to avoid file naming collisions with other students).
3. The server then waits for clients to connect by opening each FIFO for reading. There is no need to use `fork(2)` in this task, you can assume that clients connect in the same order they are specified on the command line.
4. Once a client connects, the server prints the message `"<client_name> connected."`.
5. The server continuously monitors all FIFOs for incoming messages using `poll(2)`.
   1. Once a message is received, the message counter variable is incremented by 1 and printed to the console like so `"Message <n>: \"<message>\" from <user>"`.
   2. In the beginning, the server initializes the message counter variable to `0`.
   3. If a client disconnects, the server prints the message `"<client_name> disconnected."`.
6. Once all clients are disconnected, the server prints `"All clients disconnected."`, cleans up the FIFOs, and exits.

### Client

1. A **client** is started by providing it with a name, e.g., `./client jacobi`.
2. The client will then attempt to connect to the server by opening one of the FIFOs for writing.
3. Once connected, the client continuously prompts for a string message to send.
4. Each message is then written to the FIFO. You can assume a message to be at most `PIPE_BUF` long.
5. If the message is empty, the client disconnects by closing the FIFO and exiting.

Here is an example session (each line represents a different point in time, `>` indicates user input).

```
| TERMINAL 1                      | TERMINAL 2             | TERMINAL 3              |
|---------------------------------|------------------------|-------------------------|
| > ./server jacobi riemann       |                        |                         | 
|                                 | > ./client jacobi      |                         | 
|                                 | Message:               |                         |
| jacobi connected.               |                        |                         | 
|                                 |                        | > ./client riemann      |
|                                 |                        | Message:                |
| riemann connected.              |                        |                         | 
|                                 | > Hello                |                         | 
|                                 | Message:               |                         |
| Message 1: "Hello" from jacobi  |                        |                         |
|                                 |                        | > World                 | 
|                                 |                        | Message:                |
| Message 2: "World" from riemann |                        |                         | 
|                                 | > (empty)              |                         |
| jacobi disconnected.            |                        |                         | 
|                                 |                        | > (empty)               |
| riemann disconnected.           |                        |                         | 
| All clients disconnected.       |                        |                         | 
```

Additional notes and hints:

- You can assume that all clients are connected before handling their messages.
- Your server doesn't need to support clients re-connecting, i.e. they can (and must) connect exactly once.
- Your solution **must** use `poll(2)`, **not** `select(2)`.
- Make sure to use appropriate file permissions (which we discussed in connection to `chmod`) to create and open your FIFOs.
- Your FIFOs should be **blocking**, i.e. you **must not** use `O_NONBLOCK` in your calls to `open(2)`.
- As always, make sure to properly release all allocated resources (e.g. FIFOs) upon exiting.
- Some of the functionalities required for this task might behave differently on different platforms.
  Make sure that your program is compiling and running correctly on ZID-GPL.
  - On macOS, `poll(2)` is bugged and will not return when you close the pipe from the client.
  - On Windows Subsystem for Linux (WSL) FIFOs might not work if they are created somewhere inside `/mnt/`.
    This is due to NTFS limitations.
- If you want to create multiple processes side-by-side for testing, there are several different options available to you:
  1. Open multiple terminal windows or tabs.
  2. Use shell jobs to switch between different processes: <kbd>CTRL</kbd> + <kbd>Z</kbd>, `&`, `fg`, `bg`, `jobs`, etc.
  3. Use a terminal multiplexer such as `tmux` or `screen`

Shortly answer the following questions inside a `task_3.txt` or `task_3.md` file:

* What happens in your program if two clients with the same name would join?
  If this would produce an error in your program, how could you mitigate it?
  _Note:_ You don't have to actually implement it.
* Why is it important that a message is at most `PIPE_BUF` long?
* What file permissions did you use to create and open your FIFOs? Why?

--------------

Submit your solution as a zip archive via [OLAT](https://lms.uibk.ac.at/), structured as follows, where csXXXXXX is your UIBK login name. Your zip archive **must not** contain binaries.

```text
exc04_csXXXXXX.zip
├── Makefile             # optional
├── group.txt            # optional
├── task_1
│   ├── Makefile
│   ├── arg_counter.c
│   └── task_1.txt       # or .md
├── task_2
│   ├── file_1.txt
│   ├── file_2.txt
│   ├── Makefile
│   ├── cat_cut.c
│   ├── cat_grep_cut.c
│   └── task_2.txt       # or .md
└── task_3
    ├── Makefile
    ├── client.c
    ├── server.c
    ├── common.h         # optional, if you want to share code between server/client
    └── task_3.txt       # or .md
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
