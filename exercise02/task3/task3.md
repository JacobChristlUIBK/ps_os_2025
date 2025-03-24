## Is the order of the messages consistent?
Depends on the amount of rolls.
At high roll counts it is basically random, since the time processes need to run are extremely long in proportion to the time between them being started by the parent process.
The smaller the amount of rolls, the higher the odds are the are in order however.

At a roll count of 100 they are almost in perfect order everytime on my machine.

## Can the order of these messages be predicted?
The perfect order can not be predicted.

## What does it depend on?
The amount of time the operating system gives each process to do calculations.
- Prozessplaner, Warteschlangen

## Does it matter whether you do this before or after the call to `fork()`?
Yes.
the parent thread keeps it's PID so reseeding before forking would just pass the parents PID as the seed to the child process.
