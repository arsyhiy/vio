// Эти две функции являются вспомогательными для обработки ошибок при работе с временными файлами 
// и динамическим выделением памяти. Они используют функцию die, которая, вероятно, завершает 
// программу с выводом сообщения об ошибке, если что-то пошло не так.

#include "usage.h"

#include <stdlib.h>

/* Function copyright: git */
int xmkstemp(char *template)
{
	int fd;

	fd = mkstemp(template);
	if (fd < 0)
		die("Unable to create temporary file");
	return fd;
}

void *xmalloc(size_t size)
{
	void *ret = malloc(size);
	if (!ret)
		die("Out of memory");
	return ret;
}
