#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define PATH "/dev/scdev0"
#define MAX_BUFFER_LEN 32

int main(int argc, char **argv)
{
	int fd, res;
	char buf[MAX_BUFFER_LEN] = "\0";

	fd = open(PATH, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "open %s ERROR(%d).\n", PATH, fd);
		exit(EXIT_FAILURE);
	}

	res = read(fd, buf, sizeof(buf));
	if (res < 0) {
		fprintf(stderr, "read %s ERROR(%d).\n", PATH, res);
		exit(EXIT_FAILURE);
	} else
		printf("Recieve(%s)\n", buf);

	if (argc > 1)
		res = write(fd, argv[1], strlen(argv[1]));
	else {
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "Simple char devive<%s>", PATH);
		res = write(fd, buf, strlen(buf));
	}
	if (res < 0) {
		fprintf(stderr, "write %s ERROR(%d).\n", PATH, res);
		exit(EXIT_FAILURE);
	}

	memset(buf, 0, sizeof(buf));
	res = read(fd, buf, sizeof(buf));
	if (res < 0) {
		fprintf(stderr, "read %s ERROR(%d).\n", PATH, res);
		exit(EXIT_FAILURE);
	} else
		printf("Recieve(%s)\n", buf);

	close(fd);
	exit(EXIT_SUCCESS);
}
