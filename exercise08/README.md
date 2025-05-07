# Exercise Sheet 8

In this exercise sheet, we will use **TCP sockets**, yet another form of inter-process communication (IPC). However, TCP sockets are not limited to a single machine; they may be used to exchange data between arbitrary, properly connected machines using the Internet Protocol (IP).

Specifically, we will implement a simple chat application, which will be used on a single machine.

Have a look at the man pages `ip(7)` and `socket(7)`. Here is a quick TLDR:

1. To create a TCP/IP socket, use `socket(2)` with the argument `domain` set to `PF_INET`, `type` set to `SOCK_STREAM`, and `protocol` set to `0`.
2. Next, fill out the `sockaddr_in` struct to inform the socket where to bind to, e.g.:

   ```c
   const struct sockaddr_in addr = {
     .sin_family = AF_INET,
     .sin_addr = {
       .s_addr = htonl(INADDR_ANY),
     },
     .sin_port = htons(port),
   };
   ```

A **server** application establishes client connections by

3. binding the socket to the local socket address, which is represented by the created struct, using `bind(2)`,
4. listening for incoming connections using `listen(2)`,
5. and accepting incoming connections such that a communication socket for the respective client is obtained using `accept(2)`.

On the other hand, a **client** application can connect its socket to the server's remote socket address, which is represented by the created struct, by using `connect(2)`.

**Additional notes and hints:**

- Make sure to properly close all sockets before exiting by using `close(2)`.
- `bind(2)` returning `EADDRINUSE` ("Address already in use") may indicate that you did not properly close a socket.
  You can use a different port or find another way to circumvent this for testing.
  Ultimately, you should, however, try to find the root cause of the problem.
- You **must not** use any global variables!
  However, you are allowed to `#define` preprocessor constants.
- Use `-D_DEFAULT_SOURCE` in your Makefile to compile on ZID-GPL.

## Task 1

As a first step, implement a basic **server** application, where multiple clients can dynamically connect, disconnect and send messages to the server.
The server then prints the received messages to the console, similar to the "FIFO chat app" of [task 3 of exercise sheet 4](../exercise04/README.md#task-3).

For this task, you don't need to create a separate client application.
Instead, you can use the `netcat` (`nc`) utility to establish a TCP connection to your server.
The command `nc localhost <port>` will open a prompt where you can send messages to your server and see its response.
You can exit `netcat` by pressing `CTRL+C` (`^C` in example output below), which corresponds to the client disconnecting.

The server:

- Receives at least 2 arguments,
  - The first is the `port` it should listen on.
  - The second and following are usernames representing the admins of the chat.
  - You can assume a maximum of 5 different admin usernames.
- The server starts with creating and binding the socket.
- Next, a list of admins is saved and the server spawns a _listener thread_ for accepting incoming connections.
  - The listener thread should terminate once the `/shutdown` command has been received from an admin.
    A good way of implementing this is to use the `pthread_cancel(3)` function.
- For each client (i.e. `netcat` instance) that connects, a new _client thread_ is spawned:
  - The server receives the username of the client (=first message sent by `netcat` instance) and sets a corresponding flag indicating whether the user is an admin or not. If the newly connected user is an admin, the server prints `"<username> connected (admin)."`, otherwise it prints `"<username> connected."`.
  - The client thread continuously waits for incoming messages, and upon receiving them, prints them to the console using the format `"<username>: <message>"`.
- The server waits until the listener thread and all client threads have terminated, cleans up and exits.
  When an admin user disconnects, the server prints `"<username> disconnected (admin)."`, otherwise it prints `"<username> disconnected."`.

Your chat application should support a special `/shutdown` command.
If sent by an admin, the message `/shutdown` informs the server to shut down, no longer accepting new connections. This should happen as follows:

1. The server prints `"Server is shutting down."` upon receiving the message.
2. If some clients are still connected, the server should further print `"Waiting for <N> client(s) to disconnect."`, where `N` is the number of remaining (i.e. still connected) clients.
3. The server waits for all remaining clients to disconnect, cleans up and exits.

**Example output:**

```text
TERMINAL 1                      TERMINAL 2                      TERMINAL 3                          TERMINAL 4 

> ./server 24835 jacob
Listening on port 24835.
                                > nc localhost 24835
                                > alice
alice connected.
                                                                > nc localhost 24835
                                                                > bob
bob connected.
                                                                                                    > nc localhost 24835
                                                                                                    > jacob
jacob connected (admin).
                                > hi all
alice: hi all
                                                                > what's up?
bob: what's up?
                                > /shutdown
alice: /shutdown
                                                                                                    > only I can do that!
jacob: only I can do that!
                                                                                                    > /shutdown
jacob: /shutdown
Server is shutting down. Waiting for 3 client(s) to disconnect.
                                                                >^C
bob disconnected.
                                >^C
alice disconnected.
                                                                                                    >^C
jacob disconnected (admin).
```

**Additional notes & hints:**

- You don't have to ensure that the usernames are distinct.
- You can assume that every message sent over a socket (including usernames) is at most `MSG_SIZE = 128` bytes long.
- The message received from the `netcat` instance will contain a newline character (e.g. `"alice\n"`); replace it by a terminating null-byte (i.e. `\0`).
- As always, ensure proper synchronization for all shared data structures.

## Task 2

In this task, we will write a client application for the server from the previous task. This application has to have the following behavior (similar to the previous use of `nc`):

- start it with `./client <port> <username>`.
- send the username to the server (no extra message needed by the user).
- continuously prompt for input which is sent to the server.
- when `/quit` is entered: clean up and exit.
- when `/shutdown` is entered: send a shutdown message to the server, then clean up and exit, similar to `/quit`  (similar to Task 1, the server is responsible for verifying whether the message was received from an admin).

**Example output:**

```text
TERMINAL 1                      TERMINAL 2                      TERMINAL 3

> ./server 24835 jacob
Listening on port 24835.
                                > ./client 24835 alice
alice connected.
                                                                > ./client 24835 jacob
jacob connected (admin).
                                > hi all
alice: hi all
                                                                > what's up?
jacob: what's up?
                                                                > /shutdown
jacob: /shutdown
jacob disconnected (admin).
Server is shutting down. Waiting for 1 client(s) to disconnect.
                                > /quit
alice disconnected.
```

Answer the following questions in `task_2.txt` or `task_2.md`:
- The example uses `INADDR_ANY`. Could we also use `INADDR_LOOPBACK`?
- The example uses `SOCK_STREAM`. Name two other types of sockets and briefly explain their difference compared to `SOCK_STREAM`.
- What is the range of possible ports that we assign to `addr.sin_port` in the server?
- Why is it a good idea to use a port greater or equal to `1024`?

## Task 3

Now turn your application from a "one-way shoutbox" into a real chat: Instead of just printing messages on the server, each message should also be broadcasted to all other clients.

Notably, your client now needs to be able to both read user input, as well as receive incoming messages broadcasted by the server.
Use threads to implement this functionality.

**Example output:**

```text
TERMINAL 1                      TERMINAL 2                      TERMINAL 3                      TERMINAL 4

> ./server 24835 jacob
Listening on port 24835.
                                > ./client 24835 alice
alice connected.
                                                                > ./client 24835 bob
bob connected.                  bob connected.                                                  > ./client 24835 jacob
jacob connected (admin).        jacob connected (admin).        jacob connected (admin).
                                                                > hi all
bob: hi all                     bob: hi all                                                     bob: hi all
                                > whats up?
alice: whats up?                                                alice: whats up?                alice: whats up?
                                                                > /quit
bob disconnected.               bob disconnected.                                               bob disconnected.
                                                                                                > /shutdown
jacob: /shutdown                jacob: /shutdown
jacob disconnected (admin).     jacob disconnected (admin).
Server is shutting down. Waiting for 1 client(s) to disconnect.
                                > /quit
alice disconnected.                        
```

**Additional notes & hints:**

- Again, make sure to properly synchronize access to any shared data.
- Don't overthink your solution — remember that a single socket can be used simultaneously for reading and writing from different threads.

---

Submit your solution as a zip archive via OLAT, structured as follows, where csXXXXXX is your UIBK login name. Your zip archive must not contain binaries.

```text
exc08_csXXXXXX.zip
├── Makefile             # optional
├── group.txt            # optional
├── task_1
│   ├── Makefile
│   └── server.c
├── task_2
│   ├── client.c
│   ├── common.h         # optional, if you want to share code between server/client
│   ├── Makefile
│   ├── server.c
│   └── task_2.txt       # or .md 
└── task_3
    ├── client.c
    ├── common.h         # optional, if you want to share code between server/client
    ├── Makefile
    └── server.c
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
