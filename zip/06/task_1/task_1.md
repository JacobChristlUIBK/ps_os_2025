# 1.

- Optimization flags define which things the compiler should optimize for.
	Optimization for speed is useful for machines like regular PCs where memory is there in abundance, while optimizing for codesize is useful for micro controllers or similar build targets, that have a very limited amount of memory.
- `-O2` has a slightly better execution time, in trade for a little worse compile time comapred to `O3`
(https://www.rapidtables.com/code/linux/gcc/gcc-o.html)

# 2.

One try each:

## `-02`
```
$ time ./a.out
25000000

________________________________________________________
Executed in  323.48 millis    fish           external
   usr time    4.06 secs      1.18 millis    4.06 secs
   sys time    0.10 secs      1.20 millis    0.10 secs
```

## `-03`
```
$ time ./b.out
25000000

________________________________________________________
Executed in  322.45 millis    fish           external
   usr time    4.13 secs    758.00 micros    4.13 secs
   sys time    0.08 secs    713.00 micros    0.08 secs
```

In conclution after trying each one multiple times, the actual result depends a LOT on other factors.
In the example above it is the opposite of the expected results.

# 3.

- on atomic types it is thread save since the compiler ensures that it is performed as a single step, so no race conditions accour
- `atomic_fetch_add` as a function is essentially the same as teh += binary operator as the second argument gets added to the first.
- atomic bitwise OR, atomic bitwise exclusive OR, atomic bitwise AND
