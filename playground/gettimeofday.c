#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

int	main(void)
{
	struct timeval tp;
	struct timezone tz;

	int ret = gettimeofday(&tp, &tz);
	if (ret == 0)
	{
		printf("second      : %ld\n", tp.tv_sec);
		printf("Microsecond : %d\n", tp.tv_usec);
	}
	int stopmicrosecond = 5000; // 1秒=1000000マイクロセカンド
	usleep(stopmicrosecond);
	ret = gettimeofday(&tp, &tz);
	if (ret == 0)
	{
		printf("second      : %ld\n", tp.tv_sec);
		printf("Microsecond : %d", tp.tv_usec);
	}

	else
	{
		printf("ERROR:%s\n", strerror(errno));
	}
	return (0);
}