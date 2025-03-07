#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int	main(void)
{
	char *buffer = malloc(100);
	memset(buffer, 0, 100);

	printf("buffer: %s\n", buffer);
	buffer[0] = 'a';
	printf("buffer[0]: %c\n", buffer[0]);
	printf("buffer[1]: %c\n", buffer[1]);
	free(buffer);
	return (0);
}