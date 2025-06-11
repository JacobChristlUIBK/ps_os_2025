## Is `some_library.so` the only shared library used by `some_program`?

No, it also uses 3 other shared libraries specificaly
linux-vdso.so.1 (0x00007f19dcb1d000)
	some_library.so => ./some_library.so (0x00007f19dcb12000)
	libc.so.6 => /nix/store/cg9s562sa33k78m63njfn1rw47dp9z0i-glibc-2.40-66/lib/libc.so.6 (0x00007f19dc800000)
	/lib64/ld-linux-x86-64.so.2 => /nix/store/cg9s562sa33k78m63njfn1rw47dp9z0i-glibc-2.40-66/lib64/ld-linux-x86-64.so.2 (0x00007f19dcb1f000)

## What is the difference between dynamic and static linking?

Dynamic linking, links libraries in seperate files, while static linking doesn't allow for that. Dynamic linking executes the last linking step when running a program. This also includes standard-library as can be seen one question above.

some_program is also way smaller in size.

## When would you use dynamic linking?

When you want to make a program as optimized as possible in environemnts where multiple programs require same functionality.

## When would you use static linking?

When a program needs to run in very sdpecific environments.

## What is _position independent code_, and why is it useful?

Code that can be executed independed of its position in memory.

## What is the purpose of the `LD_LIBRARY_PATH` environment variable?

It provides a list of locations to look for shared libraries.
