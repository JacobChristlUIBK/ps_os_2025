Each version was timed for 5 iterations on the zid-gpl.

| pthread mutex | 1    | 2    | 3    | 4    | 5    | avg   |
|---------------|------|------|------|------|------|-------|
| user          | 3.93 | 5.47 | 4.74 | 5.41 | 3.84 | 4.678 |
| system        | 2.38 | 3.91 | 2.71 | 3.59 | 2.68 | 3.054 |

| custom mutex | 1     | 2     | 3     | 4     | 5     | avg    |
|--------------|-------|-------|-------|-------|-------|--------|
| user         | 16.84 | 20.03 | 10.86 | 22.11 | 10.53 | 16.074 |
| system       | 0.05  | 0.00  | 0.09  | 0.00  | 0.02  | 0.018  |

The reason the system time went down so much, is because the glibc implementation of `pthread_mutex_lock` and `pthread_mutex_unlock` probably have some sort of system calls.
The increased user time probably stems from the way higher amount of loops in `c_mutex_lock`.

| custom mutex (sched_yield) | 1    | 2    | 3    | 4    | 5    | avg   |
|----------------------------|------|------|------|------|------|-------|
| user                       | 3.15 | 3.71 | 3.35 | 3.39 | 3.04 | 3.328	|
| system                     | 1.25 | 1.32 | 1.18 | 1.22 | 1.23 | 1.240 |

These times are even better than regular pthread mutexes.
This is probably because pthread mutexes have some sort of optimization using something similar to `sched_yield` but also provides more versatility and error handling, while the custom implementation does not check for any errors.
