- What difference do you notice between `SIGINT` and `SIGKILL`?

`SIGKILL` can not be caught using sigaction.

- What can the signals `SIGSTOP` and `SIGCONT` be used for?

To stop and continue processes.

- What happens when you press `CTRL + c` while the program is running (as a foreground process)?

`SIGINT` is caught.

- What happens when you press `CTRL + z` while the program is running (as a foreground process)?

It stops.

- Hypothetically, what would happen in your program if another signal arrives while your signal handler is still executing? Does it make a difference if it is the same signal or a different one?

If the same signal arrives it is ignored. Other signals are processed unless provided as `sa_mask` in `struct sigaction` according to sigaction(2).
