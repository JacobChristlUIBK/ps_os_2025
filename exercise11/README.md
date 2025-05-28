# Exercise Sheet 11

## Task 1

`some_program` is an executable that should run on most `x86_64` Linux machines (and in particular it runs on ZID-GPL).
`some_library.so` is a _shared library_ used by the executable.

The executable receives a list of numbers as input arguments.
You can run the executable inside the `task_1` directory like so:

```sh
LD_LIBRARY_PATH=. ./some_program 1 8 5 9 3
```

Unfortunately, the library contains bugs.
While the library implementation is available in [some_library.c](task_1/some_library.c), the name of the function got corrupted.

- Use `nm` to find the name of the function (its name will tell you the intended behavior).
- Read up on how GCC can be used to compile shared libraries (i.e. _shared objects_). You should start by looking at the [linking options](https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html) GCC offers.
- Fix the bug in the library function.
- Write a `Makefile` to compile the revised version of `some_library.so`.
- Verify that `some_program` now behaves as expected.
- Is `some_library.so` the only shared library used by `some_program`?
  Use `ldd` to check.

Then:

- Re-implement the functionality of `some_program` and `some_library.so` in a single program `my_program`.
- Extend your `Makefile` to compile this program as a _static_ executable (again, have a look at the [linking options](https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html) and find out how this can be done with GCC).
- Compare the sizes of `some_program` and `my_program` — what do you find?
- Verify that your executable is actually static. Use `ldd` and `file` for this.

Equipped with this knowledge, answer the following questions in `task_1.txt` or `task_1.md`:

- What is the difference between dynamic and static linking?
- When would you use dynamic linking?
- When would you use static linking?
- What is _position independent code_, and why is it useful?
- What is the purpose of the `LD_LIBRARY_PATH` environment variable?

**Note:** You can use `objdump` to disassemble a binary.
Inspecting a binary obtained from an unknown source before running it is always a good idea!

```sh
objdump --disassemble some_program --section=.text
```

**Note 2:** Linking a fully _static_ executable is not possible on ZID-GPL as a static version of the standard library is missing.
You should expect the following error when compiling on ZID-GPL:
````text
/usr/bin/ld: cannot find -lc
collect2: error: ld returned 1 exit status
make: *** [<builtin>: my_program] Error 1
````

In this case, it is expected that your code won't compile on ZID-GPL.
Try to link a fully _static_ executable on your own system instead.
Afterwards, comment the lines that are responsible for static linking in your `Makefile` before submitting,
so that the `make` command still completes without errors on ZID-GPL.

## Task 2

Dynamic linking is usually handled by the loader when running an executable. 
However, in some situations, a more manual approach is required. 
In particular, _plugin systems_ are commonly implemented this way.

Write a program called `map_string` that receives (as arguments) a string followed by a list of shared libraries that act as _plugins_ for the program.

The initial string is used as input for the first plugin. The result of the first plugin is used as input for the second plugin, and so on.

To achieve this, first read the man pages for `dlopen(3)` and `dlsym(3)`.

This kind of plugin system can be very useful for implementing so called _filter chains_. In our case, we want to implement a _filter chain_ that, according to the plugin order, modifies the input string such that the output can be reversed to the original string by running the program with the same plugins in any order. This can be very useful for implementing encryption and decryption algorithms, with the added benefit that the encryption and decryption algorithms can be extended at the time of program execution.

To do this, you have to implement the following plugins:
* `caesar_cipher.so`: Shifts all letters in the string by 13 positions. Shifting can be performed by adding the integer value 13 to a character. You might need to work with the modulo operator. Consider [this](https://en.wikipedia.org/wiki/Caesar_cipher) for more details.
* `xor_string.so`: XORs all letters in the string with the value 0x20. XOR can be performed on a character using the `^` operator.
* `reverse_string.so`: Reverses the string. 
* `rotate_string.so`: Shifts all characters by (`str_len / 2`) or (`(str_len + 1) / 2`) to the right.

For example, given those plugins:
```sh
$ LD_LIBRARY_PATH=. ./map_string "Hello World." ./caesar_cipher.so ./xor_string.so ./reverse_string.so ./rotate_string.so
./caesar_cipher.so: Uryyb Jbeyq.
./xor_string.so: uRYYB jBEYQ.
./reverse_string.so: .QYEBj BYYRu
./rotate_string.so:  BYYRu.QYEBj 
```

Running these specific plugins in any order should yield the original string:
```sh 
$ LD_LIBRARY_PATH=. ./map_string " BYYRu.QYEBj" ./rotate_string.so ./caesar_cipher.so ./reverse_string.so ./xor_string.so
./rotate_string.so: .QYEBj BYYRu
./caesar_cipher.so: .DLROw OLLEh
./reverse_string.so: hELLO wORLD.
./xor_string.so: Hello World.
```
Additionally, think of two different plugins (called `library_one` and `library_two`) similar to the ones in the example above and implement them. If you can't think of any reversible plugins, you can also implement a plugin that modifies the string in a way that it can't be reversed to the original string.

In the end, your program should be executable as follows:
```sh 
$ LD_LIBRARY_PATH=. ./map_string "<any text with arbitrary lenght>" ./library_one.so ./library_two.so [./caesar_cipher.so] [./xor_string.so] [./rotate_string.so] [./reverse_string.so]
```

## Task 3

The dynamic linker allows you to _inject_ arbitrary shared objects into any dynamically linked program using the `LD_PRELOAD` environment variable.

Use this mechanism to _wrap_ the standard library's `malloc(3)` function in your own function, using a shared library called `malloc_spy.so`.
The wrapper function prints `allocating <size> bytes` before each allocation.
The standard library's `malloc` function is still called by your wrapper and returns the expected result.
`dlsym(3)` provides a way to call the original function.

Try your wrapper function on an existing executable such as `ls`.
```sh
LD_PRELOAD=./malloc_spy.so ls
```

**Note:** Don't use `printf` as it uses `malloc` internally. Create the output string `allocating <size> bytes` by first copying `allocating` using `stpncpy`, then adding the number with the `append_number` function given below, and finally adding `bytes` using stpncpy. Print the complete message with `write(3)`.

```c
// NOTE: The return value and arguments (except `number`) of
//       this function behave similar to `stpncpy`.
char* append_number(char* dst, size_t number, size_t len) {
	if (len == 0) {
		return dst;
	}

	if (number > 9) {
		char* new_dst = append_number(dst, number / 10, len);

		if (*new_dst != '\0') {
			return new_dst;
		}

		len -= (new_dst - dst);
		dst = new_dst;
	}

	if (len-- > 0) {
		const char digit = '0' + number % 10;
		*dst = digit;
	}

	if (len > 0) {
		*(++dst) = '\0';
	}

	return dst;
}
```

---

Submit your solution as a zip archive via OLAT, structured as follows, where csXXXXXX is your UIBK login name. Your zip archive **must not** contain binaries.

```text
exc11_csXXXXXX.zip
├── Makefile             # optional
├── group.txt            # optional
├── task_1
│   ├── Makefile
│   ├── my_program.c
│   ├── some_library.c
│   └── task_1.txt       # or .md
├── task_2
│   ├── caesar_cipher.c
│   ├── Makefile
│   ├── map_string.c
│   ├── reverse_string.c
│   ├── xor_string.c
│   ├── rotate_string.c
│   ├── library_one.c
│   └── library_two.c
└── task_3
    ├── Makefile
    └── malloc_spy.c
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
