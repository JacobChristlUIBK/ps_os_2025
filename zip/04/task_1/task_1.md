## How do you set/unset environment variables in your shell?

- sh: `$ export <var>=<value>`
- fish: `$ set -x <var> <value>`

## How can you retrieve the returned *exit code* in your shell after running the program?

- sh: `$ echo $?`
- fish: `$ echo $status`

## In your shell `;`, `&&`, and `||` can be used to execute multiple commands on a single line. What are the differences between these 3 operators?

- `;`: next command is executed no matter what.
- `&&`: next command is executed if prior command exits succesfully.
- `||`: next command is only executed if first command fails.

## What is the `PATH` environment variable and why is it important?

It defines a set of paths that a shell has access to at any given moment.
Executables located at these paths are executable from anywhere without requiring writing out the full path to that executable.
