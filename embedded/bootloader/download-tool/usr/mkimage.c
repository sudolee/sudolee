#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define MAGIC_SIZE 8
#define MAGIC_STR "LEE.dt"

/*
** +-----------------+ 
** | img header      | 1 page
** +-----------------+
** | image           | n pages  
** +-----------------+
*/

struct header {
	char magic[MAGIC_SIZE];
	int headsize;

	int img_offset;
	int img_size;
};

static void usage(void)
{
}


int main(int argc, char **argv) {
	int opt;
	char *p;
	unsigned int flags;

	while ((opt = getopt(argc, argv, "o:i:")) != -1) {
		switch (opt) {
			case 'o':
				p = optarg;
				flags |= 1 << 0;
				break;

			case 'i':

			default:
				usage();
				exit(EXIT_FAILURE);
		}
	}
	printf("optind=%d\n", optind);
	if(flags & (1 <<0))
		printf("%s\n", p);

	return 0;
}
