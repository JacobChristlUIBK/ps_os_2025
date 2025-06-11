#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

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

void* malloc(size_t size) {
	char buf[128] = { '\0' };
	size_t len = 0;

	for (size_t i = size; i > 0; i >>= 1) {
		len += 1;
	}

	strncpy(buf, "allocating", 11);
	append_number(buf + 10, size, len);
	strncpy(buf + 10 + len, "bytes", 6);

	static void* (*real_malloc)(size_t) = NULL;

	if (!real_malloc) {
	}

	return ((void* (*)(size_t))dlsym(RTLD_NEXT, "malloc"))(size);
}
