#include <stdlib.h>
#include <string.h>

int	main(void)
{
	char *buffer = malloc(100);
	memset(buffer, 0, 100);
	free(buffer);
	return (0);
}