- **What does it do?**:
The `clean` target calls the clean target in each respective subfolders Makefile matching the name pattern `task?`.
The `zip` target calls `clean`, removes previous zip files created and then recursively zips all contents of the current folder excluding some hidden project files.
The default `all` target just calls `zip`.
- **How would you call it?**:
`$ make`
- **Where are you supposed to put it?**:
Same folder of where the files to zip are. (top level of final zip file)
- **Can you think of an additional use case for `make`?**:
Any repetitive tasks that require multiple commands.
