## What is the advantage of using a condition variable in this case, compared to using plain mutexes?

Condition variables reduce the time the queue is in a locked state so elements can be pushed quicker.

## When would you use condition variables?

When you only want to modify a mutex under certain conditions a high amount of times.

## What are spurious wakeups in the context of condition variables, and how can they be mitigated?

Spurious wakeups are "false" wakeups that can happen due to multiple threads waiting for a signal.

In my program i used a loop to re-check the condition:
```c
	while (myqueue_is_empty(&queue)) {
		pthread_cond_wait(&is_empty, &queue_mutex);
	}
```

## How does the program behavior differ between the two variants?
Not at all

# Benchmarks

## 250

```
$ ./task_2
...
________________________________________________________
Executed in    5.24 secs    fish           external
   usr time    6.40 secs    0.44 millis    6.40 secs
   sys time   71.42 secs    1.56 millis   71.42 secs

$ ./task_3
...
________________________________________________________
Executed in  407.79 millis    fish           external
   usr time    0.26 secs      0.00 millis    0.26 secs
   sys time    1.42 secs      1.81 millis    1.42 secs
```

## 500

```
$ ./task_2
...
________________________________________________________
Executed in   11.05 secs    fish           external
   usr time   13.71 secs    0.47 millis   13.71 secs
   sys time  154.16 secs    1.37 millis  154.16 secs

$ ./task_3
...
________________________________________________________
Executed in  413.99 millis    fish           external
   usr time    0.25 secs      0.24 millis    0.25 secs
   sys time    1.53 secs      1.19 millis    1.53 secs
```

## 750

```
$ ./task_2
...
________________________________________________________
Executed in   17.36 secs    fish           external
   usr time   21.58 secs  706.00 micros   21.58 secs
   sys time  244.36 secs  951.00 micros  244.35 secs

$ ./task_3
...
________________________________________________________
Executed in  471.73 millis    fish           external
   usr time    0.30 secs    943.00 micros    0.30 secs
   sys time    1.59 secs    957.00 micros    1.59 secs
```

## 1000

```
$ ./task_2
...
________________________________________________________
Executed in   23.10 secs    fish           external
   usr time   28.84 secs  899.00 micros   28.84 secs
   sys time  326.09 secs  924.00 micros  326.09 secs

$ ./task_3
...
________________________________________________________
Executed in  473.41 millis    fish           external
   usr time    0.31 secs      1.30 millis    0.31 secs
   sys time    1.62 secs      0.12 millis    1.62 secs
```
