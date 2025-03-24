GCC = gcc
CFLAGS = -std=c11 -Wall -Wextra

.PHONY: all
all: create_key build

.PHONY: clean
clean:
	$(RM) main key.c key.o main.o

.PHONY: build
build: main.o key.o
	echo "Linking main"
	$(GCC) $(CFLAGS) -o main $^

.PHONY: create_key
create_key:
	echo "Creating key.c"
	printf "#include \"key.h\"\n\n" > key.c
	printf "const char* get_key() {\n" >> key.c
	printf "	return \"" >> key.c
	whoami | tr -d '\n' >> key.c
	printf "\";\n" >> key.c
	printf "}\n" >> key.c

main.o: main.c
	echo "Compiling $@"
	$(GCC) $(CFLAGS) -c -o $@ $^

key.o: key.c
	echo "Compiling $@"
	$(GCC) $(CFLAGS) -c -o $@ $^
