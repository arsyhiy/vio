// Этот код содержит одну функцию — mystrscpy, которая 
// выполняет безопасное копирование строки с ограничением на размер, чтобы избежать переполнения буфера.

#include "util.h"

/* Safe zeroing, no complaining about overlap */
void mystrscpy(char *dst, const char *src, int size)
{
	if (!size)
		return;
	while (--size) {
		char c = *src++;
		if (!c)
			break;
		*dst++ = c;
	}
	*dst = 0;
}


